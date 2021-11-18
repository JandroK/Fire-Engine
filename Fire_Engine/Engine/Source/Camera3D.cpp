#include "Application.h"

// Module 
#include "Camera3D.h"
#include "Editor.h"
#include "Input.h"

#include "Globals.h" 

#include "GameObject.h"
#include "Inspector.h"

// Components
#include "Transform.h"
#include "MeshRenderer.h"

Camera3D::Camera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera3D";

	ReStartCamera();
}

void Camera3D::ReStartCamera()
{
	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	Position = float3(5.0f, 4.0f, 5.0f);
	Reference = float3(0.0f, 0.2f, 0.0f);
	LookAt(Reference);

	CalculateViewMatrix();
}

bool Camera3D::Start()
{
	LOG(LogType::L_NORMAL, "Setting up the camera");
	bool ret = true;

	return ret;
}

bool Camera3D::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Cleaning camera");

	return true;
}

update_status Camera3D::Update(float dt)
{

	return UPDATE_CONTINUE;
}

void Camera3D::CheckInputs()
{
	float3 newPos(0, 0, 0);
	float speed = 3.0f * app->GetDt();
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * app->GetDt();

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) Focus();

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	newPos -= Z * App->input->GetWheel();

	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) OrbitRotation();

	// Recalculate matrix -------------
	CalculateViewMatrix();
}

void Camera3D::OrbitRotation()
{
	float3 pivot = float3(0,0,0);
	GameObject* gameObject = App->editor->GetGameObjectSelected();
	float3 posGO = { 0, 0, 0 };

	if (gameObject != nullptr)
	{
		MeshRenderer* mesh = static_cast<MeshRenderer*>(gameObject->GetComponent(ComponentType::MESHRENDERER));
		float3 meshCenter = mesh->GetCenterPointInWorldCoords();
		posGO = meshCenter;
	}
	
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();
		float Sensitivity = 0.25f;
		app->editor->GetGameObjectSelected();
		(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && gameObject !=nullptr) ? pivot = float3(posGO.x, posGO.y, posGO.z) : pivot = Reference;
		
		Position -= pivot;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			//X = rotate(X, DeltaX, float3(0.0f, 1.0f, 0.0f)); // Right
			//Y = rotate(Y, DeltaX, float3(0.0f, 1.0f, 0.0f)); //Up
			//Z = rotate(Z, DeltaX, float3(0.0f, 1.0f, 0.0f)); // Front
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			//Y = rotate(Y, DeltaY, X);
			//Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = Z.Cross(X);
			}
		}
		Position = pivot + Z * Position.Length();
		Reference = pivot;
	}
}

void Camera3D::Focus()
{
	//Focus
	GameObject*& objSelected = static_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR))->gameObjectSelected;

	if (objSelected != nullptr)
	{
		if (MeshRenderer* mesh = static_cast<MeshRenderer*>(objSelected->GetComponent(ComponentType::MESHRENDERER)))
		{
			const float3 meshCenter = mesh->GetCenterPointInWorldCoords();
			LookAt(meshCenter);
			const float meshRadius = mesh->GetSphereRadius();

			Position = meshCenter + ((Position - meshCenter).Normalized() * meshRadius *2);
		}
		else
		{
			float3 pivot = objSelected->transform->GetPosition();
			LookAt(pivot);
		}
	}
}

void Camera3D::RecalculateProjection()
{
	/*cameraFrustum.type = FrustumType::PerspectiveFrustum;
	cameraFrustum.nearPlaneDistance = nearPlaneDistance;
	cameraFrustum.farPlaneDistance = farPlaneDistance;
	cameraFrustum.verticalFov = (verticalFOV * 3.141592 / 2) / 180.f;
	cameraFrustum.horizontalFov = 2.f * atanf(tanf(cameraFrustum.verticalFov * 0.5f) * aspectRatio);*/
}

void Camera3D::FrontView()
{
	GameObject* gameObject = App->editor->GetGameObjectSelected();
	float3 posGO = { 0, 0, 0 };
	float3 nwPos;

	if (gameObject != nullptr)
	{
		posGO = gameObject->transform->GetPosition();

		nwPos = float3(posGO.x, posGO.y, posGO.z);
		// First param: Right, 
		// Second param: UP			//With the inverted of the axes the opposite position is obtained
		// Third param: From
		Position = nwPos + float3(0, 0, -10);
		LookAt(nwPos);
	}
}

// -----------------------------------------------------------------
void Camera3D::Look(const float3&Position, const float3&Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = (float3(0.0f, 1.0f, 0.0f).Cross(Z)).Normalized();
	Y = Z.Cross(X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void Camera3D::LookAt( const float3&Spot)
{
	Reference = Spot;

	Z = (Position - Reference).Normalized();
	X = (float3(0.0f, 1.0f, 0.0f).Cross(Z)).Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void Camera3D::Move(const float3&Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void Camera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);
}
bool Camera3D::SaveConfig(JsonParser& node) const
{
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "X.x", X.x);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "X.y", X.y);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "X.z", X.z);

	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Y.x", Y.x);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Y.y", Y.y);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Y.z", Y.z);

	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Z.x", Z.x);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Z.y", Z.y);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Z.z", Z.z);

	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Reference.x", Reference.x);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Reference.y", Reference.y);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Reference.z", Reference.z);

	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Position.x", Position.x);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Position.y", Position.y);
	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "Position.z", Position.z);

	return true;
}

bool Camera3D::LoadConfig(JsonParser& node)
{
	LOG(LogType::L_NORMAL, "Setting up the camera");

	X.x = (float)node.JsonValToNumber("X.x");
	X.y = (float)node.JsonValToNumber("X.y");
	X.z = (float)node.JsonValToNumber("X.z");
	
	Y.x = (float)node.JsonValToNumber("Y.x");
	Y.y = (float)node.JsonValToNumber("Y.y");
	Y.z = (float)node.JsonValToNumber("Y.z");
	
	Z.x = (float)node.JsonValToNumber("Z.x");
	Z.y = (float)node.JsonValToNumber("Z.y");
	Z.z = (float)node.JsonValToNumber("Z.z");
	
	Position.x = (float)node.JsonValToNumber("Position.x");
	Position.y = (float)node.JsonValToNumber("Position.y");
	Position.z = (float)node.JsonValToNumber("Position.z");

	Reference.x = (float)node.JsonValToNumber("Reference.x");
	Reference.y = (float)node.JsonValToNumber("Reference.y");
	Reference.z = (float)node.JsonValToNumber("Reference.z");

	LookAt(Reference);
	//CalculateViewMatrix();

	return true;
}