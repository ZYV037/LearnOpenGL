# LearnOpenGL
LearnOpenGL

- [learnopengl](https://learnopengl.com)
- [learnopengl-cn](https://learnopengl-cn.github.io)
- [glm](https://github.com/g-truc/glm)  
  **[Caution]** sys2 mingw64, gcc8.1, c++11
  compile error:  
  'constexpr const T& glm::vec<2, T, Q>::operator[](glm::vec<2, T, Q>::length_type) const' cannot be overloaded with 'constexpr T& glm::vec<2, T, Q>::operator[](glm::vec<2, T, Q>::length_type) const'
GLM_FUNC_DECL GLM_CONSTEXPR T const& operator[](length_type i) const;
^~~~~~~~  
**Eliminate this error by ban constexpr.**
[glm/detail/setup.hpp]
#if GLM_HAS_CONSTEXPR
define GLM_CONSTEXPR /*constexpr*/
#else
define GLM_CONSTEXPR
#endif
env: msys2 mingw64, gcc8.1, c++11
- [assimp](https://github.com/assimp/assimp)  
**[Caution]** release 4.1 compile Error in msys2 mingw64 gdb8.1!!!  
version 3.1.1 works well                             
![image](https://github.com/ZYV037/LearnOpenGL/blob/master/assimp/assimp_cmake_config.png)
