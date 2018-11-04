【技术特点】
1. 公告板实现草丛和树木
2. 地形生成函数生成小岛
3. 动态顶点缓冲实现泛起涟漪的湖泊
4. 两种设摄像机模式：自由模式（P）和行走模式（O），并且摄像机可以附着在有碰撞器的物体上（F），随物体而漫游
5. 利用混合技术实现铁笼子
6. 利用几何着色器实现雨夹雪的天气和火焰
7. 两个天空盒子实现飘动的白云
8. 利用模板和深度技术实现水面倒影和平地上的影子
9. 添加了雾的效果
10.启用了Alpha裁剪优化渲染过程
11.固定帧率为400fps

【运行说明】
直接运行Task3.exe文件就行,在场景中漫游时可以通过O和P键来切换模式，对有碰撞器的物体可以通过F键将摄像机附着在上面，随着附着的物体漫游场景（就像上了游览车一样），附着之后不能移动摄相机，但可以调整观察视角。自由模式可以飞天遁地，行走模式只能贴着地面或者湖面行走。

【框架结构说明】
Application--程序负责整个程序运行的生命周期
Task3--派生自Application，用来客制化整个程序的场景
MathHelper--一些常用的函数和计算常量如PI，Max等操作
Utils--一些工具类函数和宏定义，简化代码
GameObject--所有可视化对象（如：ModelObject一般模型对象，MeshModel复杂模型对象，BoardModel公告板，Sky天空盒子，WavesModel湖泊，Terrain地形）和不可视化对象（如：Camera）的抽象基类
Component--所有功能组件的抽象基类（如：Transform，Render和Collider，由于还没有实现Script组件，所以Task3中有大量的初始化代码和更新操作代码）
Effects--各种效果类，Effect效果基类，BasicEffect一般可是对象使用的效果类，SkyEffect天空盒子使用的效果类，BoardEffect公告板使用的效果类，ParticleEffect粒子系统使用的效果类，TerrainEffect地形使用的效果类
Light--灯光材质相关的常量和辅助函数
Timer--计时器
Texture--纹理类，提供常用的纹理操作
Framework--负责整个D3D11环境的初始化和接口的封装，屏蔽掉DirectX原生API
SkyRender、BoardRender针对特殊渲染需求的渲染器组件
GeometryGenerator用来生成简单的几何体




