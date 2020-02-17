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

