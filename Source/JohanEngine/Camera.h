#ifndef CAMERA_INCLUDE
#define CAMERA_INCLUDE

#include "FXVariable.h"
#include "TimeEvent.h"
#include "Object.h"
#include "float4x4.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

struct PLANE {
	float3 normal;
	float3 point;
	float d;
};

class DLLIMPORT Camera {
	
	FXVariable* FXCamerapos;
	FXVariable* FXMatProj;
	FXVariable* FXMatProjInverse;
	
	PLANE frustumplanes[6];
	unsigned int updatecount;

	float angleH;
	float angleV;
	float ratio;
	float fov;
	float minviewdistance;
	float maxviewdistance;
	float3 pos;
	float3 lookat;
	float3 dir; // normalized
	float3 up; // [0,1,0]^T
	float3 exactup; // precies naar 'boven'
	float3 right; // crossproduct met nep-up
	float3 exactright; // cross van precies en dir
	float4x4 matView;
	float4x4 matViewInverse;
	float4x4 matProj;
	float4x4 matProjInverse;
	float4x4 matViewProj;	
					
	void Update();
	void UpdateAngles();
	void UpdateVectors();
	void UpdateMatrices();
	void UpdateFrustumPlanes();
	void SetFrustumPlane(PLANE* curplane,float3& c1,float3& c2,float3& c3);
	
	public:
		Camera();
		~Camera();
		
		TimeEvent* OnChange; // redo frustrum culling when this happens...
		
		float4x4& GetMatView();
		float4x4& GetMatViewInverse();
		float4x4& GetMatProj();
		float4x4& GetMatProjInverse();
		float4x4& GetMatViewProj();
		bool IsVisible(Object* object);
		float3 GetExactRight();
		float3 GetRight();
		float3 GetExactUp();
		float3 GetUp();
		float3 GetDir();
		float3 GetLookAt();
		float3 GetPos();
		float GetMinViewDistance();
		float GetMaxViewDistance();
		float GetAngleH();
		float GetAngleV();
		float GetRatio();
		float GetFOV();
		void SetPos(float3 pos);
		void SetLookAt(float3 lookat);
		void SetDir(float3 dir);
		void Move(float3 dir);
		void SetFOV(float fov);
		void SetRatio(float ratio);
		void SetMinViewDistance(float viewdistance);
		void SetMaxViewDistance(float viewdistance);
		void BeginUpdate();
		void EndUpdate();
		void Print();
};

extern Camera* camera;

#endif
