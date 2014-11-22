---
layout: post
title: Docker registry工作机制
category: Docker
summary: Docker registry在Docker中的位置如下图所示，运行`docker push`、`docker pull`、`docker search`时，实际上是通过docker daemon（后文简称为daemon）与docker registry（后文简称为daemon）通信。
tags: [Docker]
---

{{ page.title }}
================

{{ page.summary }}

![Registry in Docker](/imgs/docker-registry.jpg =600x200)

## registry的结构

registry主要由以下两部分组成：

1. “前端”：docker-registry基于Flask框架，为daemon提供REST API接口。代码主要在./docker-registry目录下。了解[Flask框架](http://flask.pocoo.org/docs/0.10/quickstart/)对理解整个代码框架有很大的帮助。

2. "后台"：storage驱动，为respositories和images提供存储的地方，并为“前端”提供一些接口。代码主要在./depends/docker-registry-core/docker_registry/drivers目录下。

storage驱动需要提供的接口有：get_content、put_content、stream_read、stream_write、list_directory、exists、remove、get_size，其它的在driver.Base里（如果为一个新的存储（如：HDFS）写驱动，可以从继承driver.Base类开始，编写驱动时需要注意："Drivers are expected to receive bytes and to return bytes. Don't try to decode or encode content"）。put_content与stream_write都是写文件，区别在于前者适合写小文件，后者适合写大文件（stream_write的实现使用了关键字yield），get_content和stream_read分别适合读取小文件和大文件。

有兴趣的同学可以看看file.py里的实现，当然这只是基于本地文件系统最简单的实现——普通的文件操作，例：get_content的定义如下：

{% highlight python %}
@lru.get
def get_content(self, path):
     path = self._init_path(path)
     try:
          with open(path, mode='rb') as f:
          d = f.read()
     except Exception:
          raise exceptions.FileNotFoundError('%s is not there' % path)

     return d
{% endhighlight %}

值得一提的是get_content前的装饰器@lru.get。它会以path为key从配置好的Redis中去取值，如果取到则直接返回，否则从文件去读。当然，put_content前会有一个@lru.set，会以(path,content)的形式写到Redis中。@lru.get、@lru.set、@lru.remove由./depends/docker-registry-core/docker_registry/core/lru.py提供。（注：需要将Redis配置为LRU模式：http://redis.io/topics/lru-cache)。

还有重要的一点是：**registry的“前端”是无状态的，状态（数据）都存储在storage和Redis上。**

## 运行registry

可以直接通过官方的registry image运行：

$ docker run -p 5000:5000  STORAGE_PATH=/tmp/registry -v /home/tdwadmin/registry:/tmp/registry registry

以上命令的作用是把registry server运行在docker里，通过`docker ps`可以看到。STORAGE_PATH指定image存放的路径，但这个目录存在于container里，“-v /home/tdwadmin/registry:/tmp/registry”的作用是将该container里的目录/tmp/registry映射到本地目录/home/tdwadmin/registry（请确保该目录存在，最好是空目录，方便观察）。通过[配置文件](https://github.com/docker/docker-registry/blob/master/README.md)或命令行选项可以调整该命令的行为。

打开[http://127.0.0.1:5000/](http://127.0.0.1:5000/)就可以看到界面了，不过，界面上什么也没有 :)

通过其打印的日志，我们能看出它做了哪些操作，比如：如果打印 /GET /v1/images/.../ancestry，我们就能知道它调用了相应的函数`get_image_ancestry`。

## respositories在storage上是怎么存储的

在分析`docker pull`的机制之前，我们需要了解：docker respository由哪些部分组成，在storage上是怎么存储的（以下内容以file.py的实现举例说明）。

我们刚才启动的registry里的storage是空的，我们可以向里面push一些image

### 1. 从hub.docker.io上pull ubuntu

$ docker pull ubuntu:14.04

### 2. 打上我们的标签，重要的是加上标识127.0.0.1:5000

$ docker tag ubuntu:14.04 127.0.0.1:5000/hello   

### 3. docker push

daemon通过"127.0.0.1: 5000/hello"中的'.'和':'识别出这是一个发往127.0.0.1:5000的请求，因此会将该push请求发送给我们启动的registry

<pre>
$ docker push 127.0.0.1:5000/hello

The push refers to a repository [127.0.0.1:5000/hello] (len: 1)
Sending image list
Pushing repository 127.0.0.1:5000/hello (1 tags)
511136ea3c5a: Image successfully pushed
d497ad3926c8: Image successfully pushed
ccb62158e970: Image successfully pushed
e791be0477f2: Image successfully pushed
3680052c0f5c: Image successfully pushed
22093c35d77b: Image successfully pushed
5506de2b643b: Image successfully pushed
Pushing tag for rev [5506de2b643b] on {http://127.0.0.1:5000/v1/repositories/hello/tags/latest}
</pre>

我们只push了一个image，但是显示的是有多个image被push成功了？下面会解释。

现在我们可以看看/home/tdwadmin/registry上是怎么存储它的。

<pre>
[tdwadmin@c27 registry]$ tree -L 2
.
| ---- images
|   |---- 22093c35d77bb609b9257ffb2640845ec05018e3d96cb939f68d0e19127f1723
|   |---- 3680052c0f5cf8ecb86ddf4d6ed331c89cdb691554572a80ec04724cf6ee9436
|   |---- 511136ea3c5a64f264b78b5433614aec563103b4d4702f3ba7d4d2698e22c158
|   |---- 5506de2b643be1e6febbf3b8a240760c6843244c41e12aa2f60ccbb7153d17f5
|   |---- ccb62158e97068cc05b2f0927a8acde14c64d0d363cc448238357fe221a39699
|   |---- d497ad3926c8997e1e0de74cdd5285489bb2c4acd6db15292e04bbab07047cd0
|   `---- e791be0477f28fd52f7609aed81733427d4cc0da620962d072e18ebcb32720a4
`---- repositories
`---- library

[tdwadmin@c27 registry]$ tree repositories/library/
repositories/library/
`---- hello
|---- _index_images
|---- json
|---- tag_latest
`---- taglatest_json
</pre>

我们可以看到hello在respoitories/library下，由 _index_images、json等组成。

 _index_images记录了hello（hello所有的tag）由哪些image组成的，记录的是这些image的id。一个hello会有多个image，是因为docker image用的层级（layer）的概念，如下图所示，除了Base Image，每个image都是基于其他image而制成，这就是Dockerfile里最开始那条语句如“From ubuntu”的作用，告诉docker它将基于ubuntu而制成（事实上ubuntu也是基于其它image制成的）。引入层级的概念有什么好处呢？假设image 'foo'是基于image A、image B、image C的，而image 'bar'是基于image A、image B、image D的，这样在pull image 'foo'后（此时会将image A、B、C都pull到本地），如果再pull image 'bar'，此时就只需要pull image D了。而存储时也会有相同的好处。                                                                                       

![docker-filesystems-multilayer](/imgs/docker-filesystems-multilayer.png =250x200)

respository hello上可能有多个tag，默认是hello:latest，我们可以再向registry push一个带tag的image hello:test（docker push 127.0.0.1:5000/hello:test），然后我们可以看到hello/目录下多了一个文件tag_test，它记录的是hello:test对应的image id，而tag_latest对应的是hello（也就是hello:latest）的image id。

每个image id都对应着一个image，它们在images/目录下有相应的目录。以22093c35d77bb609b9257ffb2640845ec05018e3d96cb939f68d0e19127f1723为例，它有以下内容：

[tdwadmin@c27 images]$ ls 22093c35d77bb609b9257ffb2640845ec05018e3d96cb939f68d0e19127f1723/
ancestry _checksum json layer
     
ancestry：记录该image依赖的image，如果该文件里记录的内容只有它自己的id，说明它不依赖于别的image
_checksum: image的sha256的检验值
json: image的一些描述信息
layer：实实在在起作用的数据、程序。它们是压缩后的。

现在，我们可以想想怎么根据一个respository来获取image了：(以下hello:test为例说明)

1. 从repositories/library/hello/tag_test，取出它对应的image id: 5506de2b643be1e6febbf3b8a240760c6843244c41e12aa2f60ccbb7153d17f5

2. 获取image 5506de2b643be1e6febbf3b8a240760c6843244c41e12aa2f60ccbb7153d17f5的内容，包括layer和ancestry，并根据ancestry得到它依赖的image，采取广度或深度优先的方法遍历这些image以及它们依赖的image（当然还有它们的layer）

## docker pull的工作机制

我们将以`docker pull`为例说明registry的作用，`docker push`和`docker search`的工作机制类似。

docker pull 127.0.0.1:5000/hello时，daemon会首先搜索本地是否有相应的image，如果没有则会向registry发送REST请求（daemon通过"127.0.0.1: 5000/hello"中的'.'和':'识别出这是一个发往127.0.0.1:5000的请求）。

### 1. Get  /v1/repositories/library/hello/images

获取hello的_index_images文件的内容。暂时不清楚这个值有什么用。

相应的处理函数如下所示：
{% highlight python %}
@app.route('/v1/repositories/<path:repository>/images', methods=['GET'])
@toolkit.parse_repository_name
@toolkit.requires_auth
@mirroring.source_lookup(index_route=True)
def get_repository_images(namespace, repository)
{% endhighlight %}

### 2. Get /v1/repositories/library/hello/tags

获取hello下的所有tag和其相应的image id（通过读取文件tag_tagname得到），对于image hello，返回的是 {latest: 5506de2b643be1e6febbf3b8a240760c6843244c41e12aa2f60ccbb7153d17f5, test: 5506de2b643be1e6febbf3b8a240760c6843244c41e12aa2f60ccbb7153d17f5}。

相应的处理函数如下所示：
{% highlight python %}
@app.route('/v1/repositories/<path:repository>/tags', methods=['GET'])
@toolkit.parse_repository_name
@toolkit.requires_auth
@mirroring.source_lookup_tag
def _get_tags(namespace, repository)
{% endhighlight %}

### 3. Get /v1/images/5506de2b643be1e6febbf3b8a240760c6843244c41e12aa2f60ccbb7153d17f5/layer

获取layer的内容了store.stream_read(path, bytes_range)（byte_range是由docker daemon指定的，读取指定范围的内容），registry提供了其他两种方式供我们选择：提供了nginx_x_accel_redirect（返回一个重定向到ngix的url，然后从那获取内容）和storage_redirect（返回一个重定向的url，从那获取内容，“storage_redirect: Redirect resource requested if storage engine supports this, e.g. S3 will redirect signed URLs, this can be used to offload the server.”）。

相应的处理函数如下所示：
{% highlight python %}
@app.route('/v1/images/<image_id>/layer', methods=['GET'])
@toolkit.requires_auth
@require_completion
@set_cache_headers
@mirroring.source_lookup(cache=True, stream=True)
def get_image_layer(image_id, headers):
{% endhighlight %}

### 4. Get /v1/images/5506de2b643be1e6febbf3b8a240760c6843244c41e12aa2f60ccbb7153d17f5/ancestry

获取依赖的image id。根据这些id再去遍历它们所依赖的image，并获取layer。

相应的处理函数如下所示：
{% highlight python %}
@app.route('/v1/images/<image_id>/ancestry', methods=['GET'])
@toolkit.requires_auth
@require_completion
@set_cache_headers
@mirroring.source_lookup(cache=True, stream=False)
def get_image_ancestry(image_id, headers)
{% endhighlight %}

注：为了简便，以上省略了一些过程，如：如果daemon发现该image已在本地则不用pull了。

Enjoy the code, bye!


## 参考资料

registry源码：[https://github.com/docker/docker-registry.git](https://github.com/docker/docker-registry.git)

Flask快速入门：[http://flask.pocoo.org/docs/0.10/quickstart/](http://flask.pocoo.org/docs/0.10/quickstart/)

运行registry: 1.[http://blog.docker.com/2013/07/how-to-use-your-own-registry/](http://blog.docker.com/2013/07/how-to-use-your-own-registry/)   

2.[http://blog.tankywoo.com/docker/2014/05/08/docker-3-docker-registry.html](http://blog.tankywoo.com/docker/2014/05/08/docker-3-docker-registry.html)

开发registry需要注意的地方：[https://github.com/docker/docker-registry/blob/master/CONTRIBUTING.md](https://github.com/docker/docker-registry/blob/master/CONTRIBUTING.md)

Docker源码分析：[http://www.infoq.com/cn/articles/docker-source-code-analysis-part1](http://www.infoq.com/cn/articles/docker-source-code-analysis-part1)

Where are docker images stored： [http://blog.thoward37.me/articles/where-are-docker-images-stored/](http://blog.thoward37.me/articles/where-are-docker-images-stored/)

Docker layer:  [http://docs.docker.com/terms/layer/](http://docs.docker.com/terms/layer/)

Python装饰器：１.[http://simeonfranklin.com/blog/2012/jul/1/python-decorators-in-12-steps/](http://simeonfranklin.com/blog/2012/jul/1/python-decorators-in-12-steps/)

2.[http://www.artima.com/weblogs/viewpost.jsp?thread=240808](http://www.artima.com/weblogs/viewpost.jsp?thread=240808)

Using Redis as an LRU cache:  [http://redis.io/topics/lru-cache](http://redis.io/topics/lru-cache)
