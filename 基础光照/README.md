## 2.13

基础的Phong光照

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_2_13%2023_04_06.png)

## 2.15

多光源（平行光、球面光、锥形光）

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_2_14%2014_37_12.png)

主动+漫反射+高光反射，图中错误地把未将高光分量先与物体颜色混合，不过效果出奇地可以。

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_2_15%2019_55_18.png)

多个点光源

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_2_15%2020_38_32.png)

锥形聚光

## 2.17

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_2_17%2016_51_32%20(3).png)

法向量标准化（之前漏掉了），这样反射光的亮度就不会因模型矩阵的放大而减弱了。

搞了一个大正方体，不过要在shader里面把法向量反过来，不然反射光都显示在外侧（因为所有的正方体共用一个VAO）

## 2.19

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_2_19%2011_55_56.png)

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_2_19%2012_53_03.png)

试着生成了锥体和球体的顶点法向量数据，观察光照情况。（现代管线的VAO就是厉害，渲染千万级别的三角面（10个球面，每个球面分为2 * 1080 * 540个三角形）也能稳在50帧以上，不过预先把这么多的顶点数据传到显存要花点时间）

## 2.22

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_2_21%2017_35_36.png)

天空盒，用cubemap贴图。但是不知道为什么贴出来整个旋转了90度（坐标系统没问题啊……），必须旋转模型矩阵修正

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_2_23%2013_04_45.png)

通过depthmap的方法艰难地搞出了动态点阴影（在几何着色器里面不慎把j打成i让我查了好久的错）。目前代码已经成shi山了，想做多光源阴影还是要等把代码优化一下。

此外，查到了设置垂直同步的方法。VerticalSync(bool)函数可以开关垂直同步。关了之后帧数能有500+

## 2.29

用多个深度缓冲和多个cubemap纹理成功搞出了多光源阴影（帧数直降到300+，因为写一个cubemap就需要渲染1次， 而且对每个光源都要这样做）。但是有新的问题：多光源的颜色混合，即阴影被光照之后仍然保持阴影轮廓。

## 3.10

法线贴图（从R3D材质包中随手拿了一张来用

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_3_10%2023_00_50%20(1).png)

## 3.11

添加了ImGui，调参数方便了许多

![](https://github.com/HummaWhite/GL-Try/blob/master/%E7%BA%B9%E7%90%86/md-pics/OpenGL-Try%202020_3_18%2019_44_36.png)