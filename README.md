# BCreator.io

## 简介

## Build
### Visual Studio Community 2017

### Emscripten

### CMake

## Quick Start
### Try in Console

### Check examples

### Try the Creator

## Organization Glossary
- Asset、Prefab等近是
- 关联关系：包含、引用

## 注意
- 本框架伪代码暂时未考虑线程安全。

## 模块划分

## Thinking In Nature
- Simpler, simpler and simpler! More simple, more easy. 而且也要考虑团队成员良莠不齐这个现实问题。
- Engineering Structure: Human brain & Code architecture.
- Glossary里的概念也尽量按最朴素的无专业领域人为感染的自然思维来进行定义。
- 只有两种被使用的方式：A被调用；B被继承。针对被继承性质的抽象类，放入meta中。
- 依赖关系（h文件、包的依赖）最好是个TREE，顶多只能是DAG，绝对不能有环。
- 消除二义性、歧义。
- Part可以成为别人的零件。有两种方式自己被造出来，一种compile期coding写的，另外一种是运行期由别的零件运行构建出来。
- Part都可以被重用、并且序列化起来
- 在Unity3D中，Prefab同Scene没有实质区别，所以在我们这里概念统一为Space。Space的一部分可以被存储起来（相当于Prefab），与Unity3D的Prefab不完全相同，。
- templet和Node是什么关系？别的templet同这里类似么？node是一种内部元件么。node只是一种空间的内部实现。其实是ASSET的组件。coding出来的，还是
- Package、Module等抽象概念，各人内心自定义的含义都不一样。这种模糊的用词并不科学，实质也无非想表达一个段落层次。但是现实中一般人对层次的理解同样局限于"事不过四"，所以仍旧是可选用朴素的自然思维名词来规范组织语言。我们使用包的概念，包可以有路径，也就是说包可以有层次，但是为了简便起见，在我们自己项目的范畴，也并不打算把包同多层namespace挂钩。

## 约定
- 项目的逻辑结构同磁盘物理路径结构。
- 命名中含有直属上一层种类命名的缩写。
- 有些模块是上层应用程序若需要某项功能而必须使用的到的，某些未必。对于前者头文件名及目录名加上C2前缀以明示。后者（例如一些内部数学函数、mempool等），我们并不主动推荐使用，能够在C层面开发的人一般都有自己熟悉的一套库，不要给别人太多信息以及选择，未必是好事。当然作为功能辅助库，他要用也是可以的。无论怎样，我们的包间依赖关系都需要尽量低耦合。
- 每个包有一个专门对外h文件，就好比com/combra/java/go里的接口思想一样，内部实现和对外接口的物理实现的形式可以没有关系。此h里定义好自己的依赖，用宏把h和lib装起来。详见Runtime/Meta/_defined.h。
