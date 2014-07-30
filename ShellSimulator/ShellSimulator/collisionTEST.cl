__kernel void collisionTest(
	float4 cloudPos1, __global float16* pos1, unsigned int numCloud1, 
	float4 cloudPos2, __global float16* pos2, unsigned int numCloud2,
	__global float4* outPos)
{
	uint2 id = (uint2)(get_global_id(0), get_global_id(1));
	uint width = get_global_size(0);
	uint gid = id.x + width * id.y;
	if (gid>numCloud1){
		return;
	}
	float4 myPosition = pos1[gid].s0123;
	float4 myVelocity = pos1[gid].s4567;
	float4 myForce = pos1[gid].s89AB;
	float4 opposingForce;
	int i=0;
	for(int j=0;j<numCloud2;j++){
		float4 r = (myPosition+cloudPos1) - (pos2[j].s0123+cloudPos2);
		float distSqr = r.x*r.x + r.y*r.y + r.z*r.z;
		if (distSqr <= 0.08f){
			float impulse = dot(myVelocity.xyz - pos2[j].s456, r.xyz)/(1/myPosition.s3 + 1/pos2[j].s3);
			float4 impulsePerIMass = 900.0f*impulse * normalize(r);
			myForce -= 0.2f * impulsePerIMass / myPosition.s3;
			pos2[j].s89A += 2.0f * impulsePerIMass / pos2[j].s3;
			//break;
		}
	}
	//pos1[gid].s0123 = myPosition;
	pos1[gid].s89AB = myForce;
	//pos1[gid].s89AB = myForce;
}

__kernel void resolveStructure(
	__global float16* positions, unsigned int numPoints)
{
	uint gid = get_global_id(0) + get_global_size(0) * get_global_id(1);
	if (gid>numPoints){
		return;
	}
	float16 Point = positions[gid];
	//float stabilizationFactor = 0.0f;
	//for (int i=0;i<numPoints;i++){
	//	if (i!=gid){
	//		stabilizationFactor += rsqrt(length(Point.xyz-positions[i].xyz));
	//	}
	//}
	//Point.sCDE = Point.s012+Point.s456;
	for (int i=0;i<numPoints;i++){
		if (i!=gid){
			if (length(Point.s012-positions[i].s012)<=0.5f){
				float3 diffVel = Point.s456 - positions[i].s456;
				if (diffVel.x!=0.0f || diffVel.y!=0.0f || diffVel.z!=0.0f){
					//diffVel = diffVel * (rsqrt(length(Point.xyz-positions[i].xyz)/stabilizationFactor));
					diffVel = diffVel * 0.001f*rsqrt(length(Point.xyz-positions[i].xyz));
					Point.s456 -= diffVel;
					positions[i].s456 -= diffVel;
				}
			}
		}
	}
	positions[gid] = Point;
}
__kernel void integrate(__global float16* positions, unsigned int numPoints){
	uint gid = get_global_id(0) + get_global_size(0) * get_global_id(1);
	if (gid>numPoints){
		return;
	}
	float16 Point = positions[gid];
	Point.s456 += Point.s89A;
	Point.s89A *= 0.0f;
	Point.s012 += Point.s456;
	Point.s456 *= 0.999f;
	positions[gid] = Point;
}

//0-posX
//1-posY
//2-posZ
//3-Mass
//4-velX
//5-velY
//6-velZ
//7-
//8-forceX
//9-forceY
//A-forceZ
//B-
//C-precidPosX
//D-predicPosY
//E-predicPosZ
//F-stabilization factor
