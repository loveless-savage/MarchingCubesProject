######################################################################
# Automatically generated by qmake (3.0) Sun Jul 7 20:17:28 2019
######################################################################

TEMPLATE = app
TARGET = Qtfringehead
INCLUDEPATH += . ./include ./assimp-3.3.1/include ./Freeimage ./glew/include ./glm-0.9.9-a2
QT += opengl

# Input
HEADERS += Freeimage/FreeImage.h \
           include/loadingModel.hpp \
           include/loadingShaders.hpp \
           include/OGLFramework.hpp \
           include/Qtfringehead.hpp \
           include/ShadowMapGL.hpp \
           include/type.hpp \
           glm-0.9.9-a2/glm/common.hpp \
           glm-0.9.9-a2/glm/exponential.hpp \
           glm-0.9.9-a2/glm/ext.hpp \
           glm-0.9.9-a2/glm/fwd.hpp \
           glm-0.9.9-a2/glm/geometric.hpp \
           glm-0.9.9-a2/glm/glm.hpp \
           glm-0.9.9-a2/glm/integer.hpp \
           glm-0.9.9-a2/glm/mat2x2.hpp \
           glm-0.9.9-a2/glm/mat2x3.hpp \
           glm-0.9.9-a2/glm/mat2x4.hpp \
           glm-0.9.9-a2/glm/mat3x2.hpp \
           glm-0.9.9-a2/glm/mat3x3.hpp \
           glm-0.9.9-a2/glm/mat3x4.hpp \
           glm-0.9.9-a2/glm/mat4x2.hpp \
           glm-0.9.9-a2/glm/mat4x3.hpp \
           glm-0.9.9-a2/glm/mat4x4.hpp \
           glm-0.9.9-a2/glm/matrix.hpp \
           glm-0.9.9-a2/glm/packing.hpp \
           glm-0.9.9-a2/glm/trigonometric.hpp \
           glm-0.9.9-a2/glm/vec2.hpp \
           glm-0.9.9-a2/glm/vec3.hpp \
           glm-0.9.9-a2/glm/vec4.hpp \
           glm-0.9.9-a2/glm/vector_relational.hpp \
           assimp-3.3.1/include/assimp/ai_assert.h \
           assimp-3.3.1/include/assimp/anim.h \
           assimp-3.3.1/include/assimp/camera.h \
           assimp-3.3.1/include/assimp/cexport.h \
           assimp-3.3.1/include/assimp/cfileio.h \
           assimp-3.3.1/include/assimp/cimport.h \
           assimp-3.3.1/include/assimp/color4.h \
           assimp-3.3.1/include/assimp/config.h \
           assimp-3.3.1/include/assimp/DefaultLogger.hpp \
           assimp-3.3.1/include/assimp/defs.h \
           assimp-3.3.1/include/assimp/Exporter.hpp \
           assimp-3.3.1/include/assimp/Importer.hpp \
           assimp-3.3.1/include/assimp/importerdesc.h \
           assimp-3.3.1/include/assimp/IOStream.hpp \
           assimp-3.3.1/include/assimp/IOSystem.hpp \
           assimp-3.3.1/include/assimp/light.h \
           assimp-3.3.1/include/assimp/Logger.hpp \
           assimp-3.3.1/include/assimp/LogStream.hpp \
           assimp-3.3.1/include/assimp/material.h \
           assimp-3.3.1/include/assimp/matrix3x3.h \
           assimp-3.3.1/include/assimp/matrix4x4.h \
           assimp-3.3.1/include/assimp/mesh.h \
           assimp-3.3.1/include/assimp/metadata.h \
           assimp-3.3.1/include/assimp/NullLogger.hpp \
           assimp-3.3.1/include/assimp/postprocess.h \
           assimp-3.3.1/include/assimp/ProgressHandler.hpp \
           assimp-3.3.1/include/assimp/quaternion.h \
           assimp-3.3.1/include/assimp/scene.h \
           assimp-3.3.1/include/assimp/texture.h \
           assimp-3.3.1/include/assimp/types.h \
           assimp-3.3.1/include/assimp/vector2.h \
           assimp-3.3.1/include/assimp/vector3.h \
           assimp-3.3.1/include/assimp/version.h \
           glew/include/GL/eglew.h \
           glew/include/GL/glew.h \
           glew/include/GL/glxew.h \
           glew/include/GL/wglew.h \
           glm-0.9.9-a2/glm/detail/_features.hpp \
           glm-0.9.9-a2/glm/detail/_fixes.hpp \
           glm-0.9.9-a2/glm/detail/_noise.hpp \
           glm-0.9.9-a2/glm/detail/_swizzle.hpp \
           glm-0.9.9-a2/glm/detail/_swizzle_func.hpp \
           glm-0.9.9-a2/glm/detail/_vectorize.hpp \
           glm-0.9.9-a2/glm/detail/compute_vector_relational.hpp \
           glm-0.9.9-a2/glm/detail/qualifier.hpp \
           glm-0.9.9-a2/glm/detail/setup.hpp \
           glm-0.9.9-a2/glm/detail/type_float.hpp \
           glm-0.9.9-a2/glm/detail/type_gentype.hpp \
           glm-0.9.9-a2/glm/detail/type_half.hpp \
           glm-0.9.9-a2/glm/detail/type_int.hpp \
           glm-0.9.9-a2/glm/detail/type_mat.hpp \
           glm-0.9.9-a2/glm/detail/type_mat2x2.hpp \
           glm-0.9.9-a2/glm/detail/type_mat2x3.hpp \
           glm-0.9.9-a2/glm/detail/type_mat2x4.hpp \
           glm-0.9.9-a2/glm/detail/type_mat3x2.hpp \
           glm-0.9.9-a2/glm/detail/type_mat3x3.hpp \
           glm-0.9.9-a2/glm/detail/type_mat3x4.hpp \
           glm-0.9.9-a2/glm/detail/type_mat4x2.hpp \
           glm-0.9.9-a2/glm/detail/type_mat4x3.hpp \
           glm-0.9.9-a2/glm/detail/type_mat4x4.hpp \
           glm-0.9.9-a2/glm/detail/type_vec.hpp \
           glm-0.9.9-a2/glm/detail/type_vec1.hpp \
           glm-0.9.9-a2/glm/detail/type_vec2.hpp \
           glm-0.9.9-a2/glm/detail/type_vec3.hpp \
           glm-0.9.9-a2/glm/detail/type_vec4.hpp \
           glm-0.9.9-a2/glm/ext/vec1.hpp \
           glm-0.9.9-a2/glm/ext/vector_relational.hpp \
           glm-0.9.9-a2/glm/gtc/bitfield.hpp \
           glm-0.9.9-a2/glm/gtc/color_space.hpp \
           glm-0.9.9-a2/glm/gtc/constants.hpp \
           glm-0.9.9-a2/glm/gtc/epsilon.hpp \
           glm-0.9.9-a2/glm/gtc/integer.hpp \
           glm-0.9.9-a2/glm/gtc/matrix_access.hpp \
           glm-0.9.9-a2/glm/gtc/matrix_integer.hpp \
           glm-0.9.9-a2/glm/gtc/matrix_inverse.hpp \
           glm-0.9.9-a2/glm/gtc/matrix_transform.hpp \
           glm-0.9.9-a2/glm/gtc/noise.hpp \
           glm-0.9.9-a2/glm/gtc/packing.hpp \
           glm-0.9.9-a2/glm/gtc/quaternion.hpp \
           glm-0.9.9-a2/glm/gtc/random.hpp \
           glm-0.9.9-a2/glm/gtc/reciprocal.hpp \
           glm-0.9.9-a2/glm/gtc/round.hpp \
           glm-0.9.9-a2/glm/gtc/type_aligned.hpp \
           glm-0.9.9-a2/glm/gtc/type_precision.hpp \
           glm-0.9.9-a2/glm/gtc/type_ptr.hpp \
           glm-0.9.9-a2/glm/gtc/ulp.hpp \
           glm-0.9.9-a2/glm/gtc/vec1.hpp \
           glm-0.9.9-a2/glm/gtx/associated_min_max.hpp \
           glm-0.9.9-a2/glm/gtx/bit.hpp \
           glm-0.9.9-a2/glm/gtx/closest_point.hpp \
           glm-0.9.9-a2/glm/gtx/color_encoding.hpp \
           glm-0.9.9-a2/glm/gtx/color_space.hpp \
           glm-0.9.9-a2/glm/gtx/color_space_YCoCg.hpp \
           glm-0.9.9-a2/glm/gtx/common.hpp \
           glm-0.9.9-a2/glm/gtx/compatibility.hpp \
           glm-0.9.9-a2/glm/gtx/component_wise.hpp \
           glm-0.9.9-a2/glm/gtx/dual_quaternion.hpp \
           glm-0.9.9-a2/glm/gtx/euler_angles.hpp \
           glm-0.9.9-a2/glm/gtx/extend.hpp \
           glm-0.9.9-a2/glm/gtx/extended_min_max.hpp \
           glm-0.9.9-a2/glm/gtx/exterior_product.hpp \
           glm-0.9.9-a2/glm/gtx/fast_exponential.hpp \
           glm-0.9.9-a2/glm/gtx/fast_square_root.hpp \
           glm-0.9.9-a2/glm/gtx/fast_trigonometry.hpp \
           glm-0.9.9-a2/glm/gtx/functions.hpp \
           glm-0.9.9-a2/glm/gtx/gradient_paint.hpp \
           glm-0.9.9-a2/glm/gtx/handed_coordinate_space.hpp \
           glm-0.9.9-a2/glm/gtx/hash.hpp \
           glm-0.9.9-a2/glm/gtx/integer.hpp \
           glm-0.9.9-a2/glm/gtx/intersect.hpp \
           glm-0.9.9-a2/glm/gtx/io.hpp \
           glm-0.9.9-a2/glm/gtx/log_base.hpp \
           glm-0.9.9-a2/glm/gtx/matrix_cross_product.hpp \
           glm-0.9.9-a2/glm/gtx/matrix_decompose.hpp \
           glm-0.9.9-a2/glm/gtx/matrix_factorisation.hpp \
           glm-0.9.9-a2/glm/gtx/matrix_interpolation.hpp \
           glm-0.9.9-a2/glm/gtx/matrix_major_storage.hpp \
           glm-0.9.9-a2/glm/gtx/matrix_operation.hpp \
           glm-0.9.9-a2/glm/gtx/matrix_query.hpp \
           glm-0.9.9-a2/glm/gtx/matrix_transform_2d.hpp \
           glm-0.9.9-a2/glm/gtx/mixed_product.hpp \
           glm-0.9.9-a2/glm/gtx/norm.hpp \
           glm-0.9.9-a2/glm/gtx/normal.hpp \
           glm-0.9.9-a2/glm/gtx/normalize_dot.hpp \
           glm-0.9.9-a2/glm/gtx/number_precision.hpp \
           glm-0.9.9-a2/glm/gtx/optimum_pow.hpp \
           glm-0.9.9-a2/glm/gtx/orthonormalize.hpp \
           glm-0.9.9-a2/glm/gtx/perpendicular.hpp \
           glm-0.9.9-a2/glm/gtx/polar_coordinates.hpp \
           glm-0.9.9-a2/glm/gtx/projection.hpp \
           glm-0.9.9-a2/glm/gtx/quaternion.hpp \
           glm-0.9.9-a2/glm/gtx/range.hpp \
           glm-0.9.9-a2/glm/gtx/raw_data.hpp \
           glm-0.9.9-a2/glm/gtx/rotate_normalized_axis.hpp \
           glm-0.9.9-a2/glm/gtx/rotate_vector.hpp \
           glm-0.9.9-a2/glm/gtx/scalar_multiplication.hpp \
           glm-0.9.9-a2/glm/gtx/scalar_relational.hpp \
           glm-0.9.9-a2/glm/gtx/spline.hpp \
           glm-0.9.9-a2/glm/gtx/std_based_type.hpp \
           glm-0.9.9-a2/glm/gtx/string_cast.hpp \
           glm-0.9.9-a2/glm/gtx/texture.hpp \
           glm-0.9.9-a2/glm/gtx/transform.hpp \
           glm-0.9.9-a2/glm/gtx/transform2.hpp \
           glm-0.9.9-a2/glm/gtx/type_aligned.hpp \
           glm-0.9.9-a2/glm/gtx/type_trait.hpp \
           glm-0.9.9-a2/glm/gtx/vec_swizzle.hpp \
           glm-0.9.9-a2/glm/gtx/vector_angle.hpp \
           glm-0.9.9-a2/glm/gtx/vector_query.hpp \
           glm-0.9.9-a2/glm/gtx/wrap.hpp \
           glm-0.9.9-a2/glm/simd/common.h \
           glm-0.9.9-a2/glm/simd/exponential.h \
           glm-0.9.9-a2/glm/simd/geometric.h \
           glm-0.9.9-a2/glm/simd/integer.h \
           glm-0.9.9-a2/glm/simd/matrix.h \
           glm-0.9.9-a2/glm/simd/packing.h \
           glm-0.9.9-a2/glm/simd/platform.h \
           glm-0.9.9-a2/glm/simd/trigonometric.h \
           glm-0.9.9-a2/glm/simd/vector_relational.h \
           assimp-3.3.1/include/assimp/Compiler/poppack1.h \
           assimp-3.3.1/include/assimp/Compiler/pstdint.h \
           assimp-3.3.1/include/assimp/Compiler/pushpack1.h \
           assimp-3.3.1/include/assimp/port/AndroidJNI/AndroidJNIIOSystem.h
FORMS += Qtfringehead.ui
LIBS += $$PWD/assimp-vc120-mt.dll \
        $$PWD/glew32.dll \
		$$PWD/msvcp120d.dll \
		$$PWD/msvcr120d.dll \
		$$PWD/glew/bin/Release/Win32/glew32.dll \
		$$PWD/glew/bin/Release/x64/glew32.dll
OTHER_FILES += shaders/depth_frags.c \
               shaders/depth_verts.c \
               shaders/frags.c \
               shaders/purecolor_frags.c \
               shaders/purecolor_verts.c \
               shaders/render_depth_tex_frags.c \
               shaders/render_depth_tex_verts.c \
               shaders/verts.c
SOURCES += src/loadingModel.cpp \
           src/loadingShaders.cpp \
           src/main.cpp \
           src/OGLFramework.cpp \
           src/Qtfringehead.cpp \
           src/ShadowMapGL.cpp \
           glm-0.9.9-a2/glm/detail/dummy.cpp \
           glm-0.9.9-a2/glm/detail/glm.cpp
RESOURCES += Qtfringehead.qrc
