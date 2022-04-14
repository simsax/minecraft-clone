set(PROJECT_NAME minecraft-clone)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "res/shaders/Basic.shader"
    "res/shaders/Instancing.shader"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "src/Camera.h"
    "src/Chunk.h"
    "src/ChunkManager.h"
    "src/extern/glm/common.hpp"
    "src/extern/glm/detail/_features.hpp"
    "src/extern/glm/detail/_fixes.hpp"
    "src/extern/glm/detail/_noise.hpp"
    "src/extern/glm/detail/_swizzle.hpp"
    "src/extern/glm/detail/_swizzle_func.hpp"
    "src/extern/glm/detail/_vectorize.hpp"
    "src/extern/glm/detail/compute_common.hpp"
    "src/extern/glm/detail/compute_vector_relational.hpp"
    "src/extern/glm/detail/func_common.inl"
    "src/extern/glm/detail/func_common_simd.inl"
    "src/extern/glm/detail/func_exponential.inl"
    "src/extern/glm/detail/func_exponential_simd.inl"
    "src/extern/glm/detail/func_geometric.inl"
    "src/extern/glm/detail/func_geometric_simd.inl"
    "src/extern/glm/detail/func_integer.inl"
    "src/extern/glm/detail/func_integer_simd.inl"
    "src/extern/glm/detail/func_matrix.inl"
    "src/extern/glm/detail/func_matrix_simd.inl"
    "src/extern/glm/detail/func_packing.inl"
    "src/extern/glm/detail/func_packing_simd.inl"
    "src/extern/glm/detail/func_trigonometric.inl"
    "src/extern/glm/detail/func_trigonometric_simd.inl"
    "src/extern/glm/detail/func_vector_relational.inl"
    "src/extern/glm/detail/func_vector_relational_simd.inl"
    "src/extern/glm/detail/qualifier.hpp"
    "src/extern/glm/detail/setup.hpp"
    "src/extern/glm/detail/type_float.hpp"
    "src/extern/glm/detail/type_half.hpp"
    "src/extern/glm/detail/type_half.inl"
    "src/extern/glm/detail/type_mat2x2.hpp"
    "src/extern/glm/detail/type_mat2x2.inl"
    "src/extern/glm/detail/type_mat2x3.hpp"
    "src/extern/glm/detail/type_mat2x3.inl"
    "src/extern/glm/detail/type_mat2x4.hpp"
    "src/extern/glm/detail/type_mat2x4.inl"
    "src/extern/glm/detail/type_mat3x2.hpp"
    "src/extern/glm/detail/type_mat3x2.inl"
    "src/extern/glm/detail/type_mat3x3.hpp"
    "src/extern/glm/detail/type_mat3x3.inl"
    "src/extern/glm/detail/type_mat3x4.hpp"
    "src/extern/glm/detail/type_mat3x4.inl"
    "src/extern/glm/detail/type_mat4x2.hpp"
    "src/extern/glm/detail/type_mat4x2.inl"
    "src/extern/glm/detail/type_mat4x3.hpp"
    "src/extern/glm/detail/type_mat4x3.inl"
    "src/extern/glm/detail/type_mat4x4.hpp"
    "src/extern/glm/detail/type_mat4x4.inl"
    "src/extern/glm/detail/type_mat4x4_simd.inl"
    "src/extern/glm/detail/type_quat.hpp"
    "src/extern/glm/detail/type_quat.inl"
    "src/extern/glm/detail/type_quat_simd.inl"
    "src/extern/glm/detail/type_vec1.hpp"
    "src/extern/glm/detail/type_vec1.inl"
    "src/extern/glm/detail/type_vec2.hpp"
    "src/extern/glm/detail/type_vec2.inl"
    "src/extern/glm/detail/type_vec3.hpp"
    "src/extern/glm/detail/type_vec3.inl"
    "src/extern/glm/detail/type_vec4.hpp"
    "src/extern/glm/detail/type_vec4.inl"
    "src/extern/glm/detail/type_vec4_simd.inl"
    "src/extern/glm/exponential.hpp"
    "src/extern/glm/ext.hpp"
    "src/extern/glm/ext/matrix_clip_space.hpp"
    "src/extern/glm/ext/matrix_clip_space.inl"
    "src/extern/glm/ext/matrix_common.hpp"
    "src/extern/glm/ext/matrix_common.inl"
    "src/extern/glm/ext/matrix_double2x2.hpp"
    "src/extern/glm/ext/matrix_double2x2_precision.hpp"
    "src/extern/glm/ext/matrix_double2x3.hpp"
    "src/extern/glm/ext/matrix_double2x3_precision.hpp"
    "src/extern/glm/ext/matrix_double2x4.hpp"
    "src/extern/glm/ext/matrix_double2x4_precision.hpp"
    "src/extern/glm/ext/matrix_double3x2.hpp"
    "src/extern/glm/ext/matrix_double3x2_precision.hpp"
    "src/extern/glm/ext/matrix_double3x3.hpp"
    "src/extern/glm/ext/matrix_double3x3_precision.hpp"
    "src/extern/glm/ext/matrix_double3x4.hpp"
    "src/extern/glm/ext/matrix_double3x4_precision.hpp"
    "src/extern/glm/ext/matrix_double4x2.hpp"
    "src/extern/glm/ext/matrix_double4x2_precision.hpp"
    "src/extern/glm/ext/matrix_double4x3.hpp"
    "src/extern/glm/ext/matrix_double4x3_precision.hpp"
    "src/extern/glm/ext/matrix_double4x4.hpp"
    "src/extern/glm/ext/matrix_double4x4_precision.hpp"
    "src/extern/glm/ext/matrix_float2x2.hpp"
    "src/extern/glm/ext/matrix_float2x2_precision.hpp"
    "src/extern/glm/ext/matrix_float2x3.hpp"
    "src/extern/glm/ext/matrix_float2x3_precision.hpp"
    "src/extern/glm/ext/matrix_float2x4.hpp"
    "src/extern/glm/ext/matrix_float2x4_precision.hpp"
    "src/extern/glm/ext/matrix_float3x2.hpp"
    "src/extern/glm/ext/matrix_float3x2_precision.hpp"
    "src/extern/glm/ext/matrix_float3x3.hpp"
    "src/extern/glm/ext/matrix_float3x3_precision.hpp"
    "src/extern/glm/ext/matrix_float3x4.hpp"
    "src/extern/glm/ext/matrix_float3x4_precision.hpp"
    "src/extern/glm/ext/matrix_float4x2.hpp"
    "src/extern/glm/ext/matrix_float4x2_precision.hpp"
    "src/extern/glm/ext/matrix_float4x3.hpp"
    "src/extern/glm/ext/matrix_float4x3_precision.hpp"
    "src/extern/glm/ext/matrix_float4x4.hpp"
    "src/extern/glm/ext/matrix_float4x4_precision.hpp"
    "src/extern/glm/ext/matrix_int2x2.hpp"
    "src/extern/glm/ext/matrix_int2x2_sized.hpp"
    "src/extern/glm/ext/matrix_int2x3.hpp"
    "src/extern/glm/ext/matrix_int2x3_sized.hpp"
    "src/extern/glm/ext/matrix_int2x4.hpp"
    "src/extern/glm/ext/matrix_int2x4_sized.hpp"
    "src/extern/glm/ext/matrix_int3x2.hpp"
    "src/extern/glm/ext/matrix_int3x2_sized.hpp"
    "src/extern/glm/ext/matrix_int3x3.hpp"
    "src/extern/glm/ext/matrix_int3x3_sized.hpp"
    "src/extern/glm/ext/matrix_int3x4.hpp"
    "src/extern/glm/ext/matrix_int3x4_sized.hpp"
    "src/extern/glm/ext/matrix_int4x2.hpp"
    "src/extern/glm/ext/matrix_int4x2_sized.hpp"
    "src/extern/glm/ext/matrix_int4x3.hpp"
    "src/extern/glm/ext/matrix_int4x3_sized.hpp"
    "src/extern/glm/ext/matrix_int4x4.hpp"
    "src/extern/glm/ext/matrix_int4x4_sized.hpp"
    "src/extern/glm/ext/matrix_projection.hpp"
    "src/extern/glm/ext/matrix_projection.inl"
    "src/extern/glm/ext/matrix_relational.hpp"
    "src/extern/glm/ext/matrix_relational.inl"
    "src/extern/glm/ext/matrix_transform.hpp"
    "src/extern/glm/ext/matrix_transform.inl"
    "src/extern/glm/ext/matrix_uint2x2.hpp"
    "src/extern/glm/ext/matrix_uint2x2_sized.hpp"
    "src/extern/glm/ext/matrix_uint2x3.hpp"
    "src/extern/glm/ext/matrix_uint2x3_sized.hpp"
    "src/extern/glm/ext/matrix_uint2x4.hpp"
    "src/extern/glm/ext/matrix_uint2x4_sized.hpp"
    "src/extern/glm/ext/matrix_uint3x2.hpp"
    "src/extern/glm/ext/matrix_uint3x2_sized.hpp"
    "src/extern/glm/ext/matrix_uint3x3.hpp"
    "src/extern/glm/ext/matrix_uint3x3_sized.hpp"
    "src/extern/glm/ext/matrix_uint3x4.hpp"
    "src/extern/glm/ext/matrix_uint3x4_sized.hpp"
    "src/extern/glm/ext/matrix_uint4x2.hpp"
    "src/extern/glm/ext/matrix_uint4x2_sized.hpp"
    "src/extern/glm/ext/matrix_uint4x3.hpp"
    "src/extern/glm/ext/matrix_uint4x3_sized.hpp"
    "src/extern/glm/ext/matrix_uint4x4.hpp"
    "src/extern/glm/ext/matrix_uint4x4_sized.hpp"
    "src/extern/glm/ext/quaternion_common.hpp"
    "src/extern/glm/ext/quaternion_common.inl"
    "src/extern/glm/ext/quaternion_common_simd.inl"
    "src/extern/glm/ext/quaternion_double.hpp"
    "src/extern/glm/ext/quaternion_double_precision.hpp"
    "src/extern/glm/ext/quaternion_exponential.hpp"
    "src/extern/glm/ext/quaternion_exponential.inl"
    "src/extern/glm/ext/quaternion_float.hpp"
    "src/extern/glm/ext/quaternion_float_precision.hpp"
    "src/extern/glm/ext/quaternion_geometric.hpp"
    "src/extern/glm/ext/quaternion_geometric.inl"
    "src/extern/glm/ext/quaternion_relational.hpp"
    "src/extern/glm/ext/quaternion_relational.inl"
    "src/extern/glm/ext/quaternion_transform.hpp"
    "src/extern/glm/ext/quaternion_transform.inl"
    "src/extern/glm/ext/quaternion_trigonometric.hpp"
    "src/extern/glm/ext/quaternion_trigonometric.inl"
    "src/extern/glm/ext/scalar_common.hpp"
    "src/extern/glm/ext/scalar_common.inl"
    "src/extern/glm/ext/scalar_constants.hpp"
    "src/extern/glm/ext/scalar_constants.inl"
    "src/extern/glm/ext/scalar_int_sized.hpp"
    "src/extern/glm/ext/scalar_integer.hpp"
    "src/extern/glm/ext/scalar_integer.inl"
    "src/extern/glm/ext/scalar_packing.hpp"
    "src/extern/glm/ext/scalar_packing.inl"
    "src/extern/glm/ext/scalar_relational.hpp"
    "src/extern/glm/ext/scalar_relational.inl"
    "src/extern/glm/ext/scalar_uint_sized.hpp"
    "src/extern/glm/ext/scalar_ulp.hpp"
    "src/extern/glm/ext/scalar_ulp.inl"
    "src/extern/glm/ext/vector_bool1.hpp"
    "src/extern/glm/ext/vector_bool1_precision.hpp"
    "src/extern/glm/ext/vector_bool2.hpp"
    "src/extern/glm/ext/vector_bool2_precision.hpp"
    "src/extern/glm/ext/vector_bool3.hpp"
    "src/extern/glm/ext/vector_bool3_precision.hpp"
    "src/extern/glm/ext/vector_bool4.hpp"
    "src/extern/glm/ext/vector_bool4_precision.hpp"
    "src/extern/glm/ext/vector_common.hpp"
    "src/extern/glm/ext/vector_common.inl"
    "src/extern/glm/ext/vector_double1.hpp"
    "src/extern/glm/ext/vector_double1_precision.hpp"
    "src/extern/glm/ext/vector_double2.hpp"
    "src/extern/glm/ext/vector_double2_precision.hpp"
    "src/extern/glm/ext/vector_double3.hpp"
    "src/extern/glm/ext/vector_double3_precision.hpp"
    "src/extern/glm/ext/vector_double4.hpp"
    "src/extern/glm/ext/vector_double4_precision.hpp"
    "src/extern/glm/ext/vector_float1.hpp"
    "src/extern/glm/ext/vector_float1_precision.hpp"
    "src/extern/glm/ext/vector_float2.hpp"
    "src/extern/glm/ext/vector_float2_precision.hpp"
    "src/extern/glm/ext/vector_float3.hpp"
    "src/extern/glm/ext/vector_float3_precision.hpp"
    "src/extern/glm/ext/vector_float4.hpp"
    "src/extern/glm/ext/vector_float4_precision.hpp"
    "src/extern/glm/ext/vector_int1.hpp"
    "src/extern/glm/ext/vector_int1_sized.hpp"
    "src/extern/glm/ext/vector_int2.hpp"
    "src/extern/glm/ext/vector_int2_sized.hpp"
    "src/extern/glm/ext/vector_int3.hpp"
    "src/extern/glm/ext/vector_int3_sized.hpp"
    "src/extern/glm/ext/vector_int4.hpp"
    "src/extern/glm/ext/vector_int4_sized.hpp"
    "src/extern/glm/ext/vector_integer.hpp"
    "src/extern/glm/ext/vector_integer.inl"
    "src/extern/glm/ext/vector_packing.hpp"
    "src/extern/glm/ext/vector_packing.inl"
    "src/extern/glm/ext/vector_relational.hpp"
    "src/extern/glm/ext/vector_relational.inl"
    "src/extern/glm/ext/vector_uint1.hpp"
    "src/extern/glm/ext/vector_uint1_sized.hpp"
    "src/extern/glm/ext/vector_uint2.hpp"
    "src/extern/glm/ext/vector_uint2_sized.hpp"
    "src/extern/glm/ext/vector_uint3.hpp"
    "src/extern/glm/ext/vector_uint3_sized.hpp"
    "src/extern/glm/ext/vector_uint4.hpp"
    "src/extern/glm/ext/vector_uint4_sized.hpp"
    "src/extern/glm/ext/vector_ulp.hpp"
    "src/extern/glm/ext/vector_ulp.inl"
    "src/extern/glm/fwd.hpp"
    "src/extern/glm/geometric.hpp"
    "src/extern/glm/glm.hpp"
    "src/extern/glm/gtc/bitfield.hpp"
    "src/extern/glm/gtc/bitfield.inl"
    "src/extern/glm/gtc/color_space.hpp"
    "src/extern/glm/gtc/color_space.inl"
    "src/extern/glm/gtc/constants.hpp"
    "src/extern/glm/gtc/constants.inl"
    "src/extern/glm/gtc/epsilon.hpp"
    "src/extern/glm/gtc/epsilon.inl"
    "src/extern/glm/gtc/integer.hpp"
    "src/extern/glm/gtc/integer.inl"
    "src/extern/glm/gtc/matrix_access.hpp"
    "src/extern/glm/gtc/matrix_access.inl"
    "src/extern/glm/gtc/matrix_integer.hpp"
    "src/extern/glm/gtc/matrix_inverse.hpp"
    "src/extern/glm/gtc/matrix_inverse.inl"
    "src/extern/glm/gtc/matrix_transform.hpp"
    "src/extern/glm/gtc/matrix_transform.inl"
    "src/extern/glm/gtc/noise.hpp"
    "src/extern/glm/gtc/noise.inl"
    "src/extern/glm/gtc/packing.hpp"
    "src/extern/glm/gtc/packing.inl"
    "src/extern/glm/gtc/quaternion.hpp"
    "src/extern/glm/gtc/quaternion.inl"
    "src/extern/glm/gtc/quaternion_simd.inl"
    "src/extern/glm/gtc/random.hpp"
    "src/extern/glm/gtc/random.inl"
    "src/extern/glm/gtc/reciprocal.hpp"
    "src/extern/glm/gtc/reciprocal.inl"
    "src/extern/glm/gtc/round.hpp"
    "src/extern/glm/gtc/round.inl"
    "src/extern/glm/gtc/type_aligned.hpp"
    "src/extern/glm/gtc/type_precision.hpp"
    "src/extern/glm/gtc/type_precision.inl"
    "src/extern/glm/gtc/type_ptr.hpp"
    "src/extern/glm/gtc/type_ptr.inl"
    "src/extern/glm/gtc/ulp.hpp"
    "src/extern/glm/gtc/ulp.inl"
    "src/extern/glm/gtc/vec1.hpp"
    "src/extern/glm/gtx/associated_min_max.hpp"
    "src/extern/glm/gtx/associated_min_max.inl"
    "src/extern/glm/gtx/bit.hpp"
    "src/extern/glm/gtx/bit.inl"
    "src/extern/glm/gtx/closest_point.hpp"
    "src/extern/glm/gtx/closest_point.inl"
    "src/extern/glm/gtx/color_encoding.hpp"
    "src/extern/glm/gtx/color_encoding.inl"
    "src/extern/glm/gtx/color_space.hpp"
    "src/extern/glm/gtx/color_space.inl"
    "src/extern/glm/gtx/color_space_YCoCg.hpp"
    "src/extern/glm/gtx/color_space_YCoCg.inl"
    "src/extern/glm/gtx/common.hpp"
    "src/extern/glm/gtx/common.inl"
    "src/extern/glm/gtx/compatibility.hpp"
    "src/extern/glm/gtx/compatibility.inl"
    "src/extern/glm/gtx/component_wise.hpp"
    "src/extern/glm/gtx/component_wise.inl"
    "src/extern/glm/gtx/dual_quaternion.hpp"
    "src/extern/glm/gtx/dual_quaternion.inl"
    "src/extern/glm/gtx/easing.hpp"
    "src/extern/glm/gtx/easing.inl"
    "src/extern/glm/gtx/euler_angles.hpp"
    "src/extern/glm/gtx/euler_angles.inl"
    "src/extern/glm/gtx/extend.hpp"
    "src/extern/glm/gtx/extend.inl"
    "src/extern/glm/gtx/extended_min_max.hpp"
    "src/extern/glm/gtx/extended_min_max.inl"
    "src/extern/glm/gtx/exterior_product.hpp"
    "src/extern/glm/gtx/exterior_product.inl"
    "src/extern/glm/gtx/fast_exponential.hpp"
    "src/extern/glm/gtx/fast_exponential.inl"
    "src/extern/glm/gtx/fast_square_root.hpp"
    "src/extern/glm/gtx/fast_square_root.inl"
    "src/extern/glm/gtx/fast_trigonometry.hpp"
    "src/extern/glm/gtx/fast_trigonometry.inl"
    "src/extern/glm/gtx/float_notmalize.inl"
    "src/extern/glm/gtx/functions.hpp"
    "src/extern/glm/gtx/functions.inl"
    "src/extern/glm/gtx/gradient_paint.hpp"
    "src/extern/glm/gtx/gradient_paint.inl"
    "src/extern/glm/gtx/handed_coordinate_space.hpp"
    "src/extern/glm/gtx/handed_coordinate_space.inl"
    "src/extern/glm/gtx/hash.hpp"
    "src/extern/glm/gtx/hash.inl"
    "src/extern/glm/gtx/integer.hpp"
    "src/extern/glm/gtx/integer.inl"
    "src/extern/glm/gtx/intersect.hpp"
    "src/extern/glm/gtx/intersect.inl"
    "src/extern/glm/gtx/io.hpp"
    "src/extern/glm/gtx/io.inl"
    "src/extern/glm/gtx/log_base.hpp"
    "src/extern/glm/gtx/log_base.inl"
    "src/extern/glm/gtx/matrix_cross_product.hpp"
    "src/extern/glm/gtx/matrix_cross_product.inl"
    "src/extern/glm/gtx/matrix_decompose.hpp"
    "src/extern/glm/gtx/matrix_decompose.inl"
    "src/extern/glm/gtx/matrix_factorisation.hpp"
    "src/extern/glm/gtx/matrix_factorisation.inl"
    "src/extern/glm/gtx/matrix_interpolation.hpp"
    "src/extern/glm/gtx/matrix_interpolation.inl"
    "src/extern/glm/gtx/matrix_major_storage.hpp"
    "src/extern/glm/gtx/matrix_major_storage.inl"
    "src/extern/glm/gtx/matrix_operation.hpp"
    "src/extern/glm/gtx/matrix_operation.inl"
    "src/extern/glm/gtx/matrix_query.hpp"
    "src/extern/glm/gtx/matrix_query.inl"
    "src/extern/glm/gtx/matrix_transform_2d.hpp"
    "src/extern/glm/gtx/matrix_transform_2d.inl"
    "src/extern/glm/gtx/mixed_product.hpp"
    "src/extern/glm/gtx/mixed_product.inl"
    "src/extern/glm/gtx/norm.hpp"
    "src/extern/glm/gtx/norm.inl"
    "src/extern/glm/gtx/normal.hpp"
    "src/extern/glm/gtx/normal.inl"
    "src/extern/glm/gtx/normalize_dot.hpp"
    "src/extern/glm/gtx/normalize_dot.inl"
    "src/extern/glm/gtx/number_precision.hpp"
    "src/extern/glm/gtx/number_precision.inl"
    "src/extern/glm/gtx/optimum_pow.hpp"
    "src/extern/glm/gtx/optimum_pow.inl"
    "src/extern/glm/gtx/orthonormalize.hpp"
    "src/extern/glm/gtx/orthonormalize.inl"
    "src/extern/glm/gtx/perpendicular.hpp"
    "src/extern/glm/gtx/perpendicular.inl"
    "src/extern/glm/gtx/polar_coordinates.hpp"
    "src/extern/glm/gtx/polar_coordinates.inl"
    "src/extern/glm/gtx/projection.hpp"
    "src/extern/glm/gtx/projection.inl"
    "src/extern/glm/gtx/quaternion.hpp"
    "src/extern/glm/gtx/quaternion.inl"
    "src/extern/glm/gtx/range.hpp"
    "src/extern/glm/gtx/raw_data.hpp"
    "src/extern/glm/gtx/raw_data.inl"
    "src/extern/glm/gtx/rotate_normalized_axis.hpp"
    "src/extern/glm/gtx/rotate_normalized_axis.inl"
    "src/extern/glm/gtx/rotate_vector.hpp"
    "src/extern/glm/gtx/rotate_vector.inl"
    "src/extern/glm/gtx/scalar_multiplication.hpp"
    "src/extern/glm/gtx/scalar_relational.hpp"
    "src/extern/glm/gtx/scalar_relational.inl"
    "src/extern/glm/gtx/spline.hpp"
    "src/extern/glm/gtx/spline.inl"
    "src/extern/glm/gtx/std_based_type.hpp"
    "src/extern/glm/gtx/std_based_type.inl"
    "src/extern/glm/gtx/string_cast.hpp"
    "src/extern/glm/gtx/string_cast.inl"
    "src/extern/glm/gtx/texture.hpp"
    "src/extern/glm/gtx/texture.inl"
    "src/extern/glm/gtx/transform.hpp"
    "src/extern/glm/gtx/transform.inl"
    "src/extern/glm/gtx/transform2.hpp"
    "src/extern/glm/gtx/transform2.inl"
    "src/extern/glm/gtx/type_aligned.hpp"
    "src/extern/glm/gtx/type_aligned.inl"
    "src/extern/glm/gtx/type_trait.hpp"
    "src/extern/glm/gtx/type_trait.inl"
    "src/extern/glm/gtx/vec_swizzle.hpp"
    "src/extern/glm/gtx/vector_angle.hpp"
    "src/extern/glm/gtx/vector_angle.inl"
    "src/extern/glm/gtx/vector_query.hpp"
    "src/extern/glm/gtx/vector_query.inl"
    "src/extern/glm/gtx/wrap.hpp"
    "src/extern/glm/gtx/wrap.inl"
    "src/extern/glm/integer.hpp"
    "src/extern/glm/mat2x2.hpp"
    "src/extern/glm/mat2x3.hpp"
    "src/extern/glm/mat2x4.hpp"
    "src/extern/glm/mat3x2.hpp"
    "src/extern/glm/mat3x3.hpp"
    "src/extern/glm/mat3x4.hpp"
    "src/extern/glm/mat4x2.hpp"
    "src/extern/glm/mat4x3.hpp"
    "src/extern/glm/mat4x4.hpp"
    "src/extern/glm/matrix.hpp"
    "src/extern/glm/packing.hpp"
    "src/extern/glm/simd/common.h"
    "src/extern/glm/simd/exponential.h"
    "src/extern/glm/simd/geometric.h"
    "src/extern/glm/simd/integer.h"
    "src/extern/glm/simd/matrix.h"
    "src/extern/glm/simd/neon.h"
    "src/extern/glm/simd/packing.h"
    "src/extern/glm/simd/platform.h"
    "src/extern/glm/simd/trigonometric.h"
    "src/extern/glm/simd/vector_relational.h"
    "src/extern/glm/trigonometric.hpp"
    "src/extern/glm/vec2.hpp"
    "src/extern/glm/vec3.hpp"
    "src/extern/glm/vec4.hpp"
    "src/extern/glm/vector_relational.hpp"
    "src/extern/imgui/imconfig.h"
    "src/extern/imgui/imgui.h"
    "src/extern/imgui/imgui_impl_glfw.h"
    "src/extern/imgui/imgui_impl_opengl3.h"
    "src/extern/imgui/imgui_impl_opengl3_loader.h"
    "src/extern/imgui/imgui_internal.h"
    "src/extern/imgui/imstb_rectpack.h"
    "src/extern/imgui/imstb_textedit.h"
    "src/extern/imgui/imstb_truetype.h"
    "src/extern/stb_image/stb_image.h"
    "src/Game.h"
    "src/IndexBuffer.h"
    "src/Matrix.hpp"
    "src/Noise.h"
    "src/Renderer.h"
    "src/Shader.h"
    "src/Texture.h"
    "src/ThreadPool.hpp"
    "src/VertexArray.h"
    "src/VertexBuffer.h"
    "src/VertexBufferLayout.hpp"
    "src/Window.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "src/Camera.cpp"
    "src/Chunk.cpp"
    "src/ChunkManager.cpp"
    "src/extern/glm/detail/glm.cpp"
    "src/extern/imgui/imgui.cpp"
    "src/extern/imgui/imgui_demo.cpp"
    "src/extern/imgui/imgui_draw.cpp"
    "src/extern/imgui/imgui_impl_glfw.cpp"
    "src/extern/imgui/imgui_impl_opengl3.cpp"
    "src/extern/imgui/imgui_tables.cpp"
    "src/extern/imgui/imgui_widgets.cpp"
    "src/extern/stb_image/stb_image.cpp"
    "src/Game.cpp"
    "src/IndexBuffer.cpp"
    "src/main.cpp"
    "src/Noise.cpp"
    "src/Renderer.cpp"
    "src/Shader.cpp"
    "src/Texture.cpp"
    "src/VertexArray.cpp"
    "src/VertexBuffer.cpp"
    "src/Window.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${no_group_source_files}
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE minecraft-clone)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src;"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/media/sax/Shared/dev/minecraft-clone/external/GLFW/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/media/sax/Shared/dev/minecraft-clone/external/GLEW/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src;"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/media/sax/Shared/dev/minecraft-clone/external/GLFW/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/media/sax/Shared/dev/minecraft-clone/external/GLEW/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "GLEW_STATIC;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "GLEW_STATIC;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "opengl32;"
        "glfw3;"
        "glew32s;"
        "user32;"
        "Gdi32;"
        "Shell32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "opengl32;"
        "glfw3;"
        "glew32s;"
        "user32;"
        "Gdi32;"
        "Shell32"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/external/GLFW/lib-vc2022;"
        "${CMAKE_SOURCE_DIR}/external/GLEW/lib/Release/Win32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/external/GLFW/lib-vc2022;"
        "${CMAKE_SOURCE_DIR}/external/GLEW/lib/Release/Win32"
    )
endif()

