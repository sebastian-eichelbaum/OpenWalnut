
__kernel renderGlyphs(__constant float originX,__constant float originY,__constant float originZ,
					  __constant float llX,__constant float llY,__constant float llZ,
					  __constant float xx,__constant float xy,__constant float xz,
					  __constant float yx,__constant float yy,__constant float yz,
					  __constant float near,__constant float far,
					  __constant uint width,__constant uint height,
					  __global float* tensorData,
					  __constant uint numOfTensors,__constant uint numOfCoeffs,
					  __write_only image2d_t color,__write_only image2d_t depth)
{
	if (get_global_id(0) >= width) return;
	if (get_global_id(1) >= height) return;

	__local float4 direction = (float4)(llX,llY,llZ);// array copies 4 bank access ?

	float ratioX = (float)get_global_id(0) / (float)width;
	float ratioY = (float)get_global_id(1) / (float)height;

	float4 initialPoint = (float4)
	(
		ox + ratioX * xx + ratioY * yx,
		oy + ratioX * xy + ratioY * yy,
		oz + ratioX * xz + ratioY * yz,
		0.0
	);

	// etc. pp.

	write_imagef(color,(int2)(get_global_id(0),get_global_id(1)),(float4)(1.0,0.0,0.0,1.0));
	write_imagef(depth,(int2)(get_global_id(0),get_global_id(1)),(float4)(0.0,0.0,0.0,0.8));
}