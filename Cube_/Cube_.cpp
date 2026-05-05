#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include <raylib.h>
#include <iostream>
#include <raymath.h>
#include <serialib.h>

class CUBE
{
private:
	Vector3 position, size, rotationAxis, scale;
	Mesh mesh;
	Matrix rotation;
	Model model;
	float rotationAngle;
public:
	CUBE(Vector3 Postion, Vector3 Size, Vector3 RotationAxis, Vector3 Scale, Mesh Mesh_, Matrix Rotation, float Rangle) : 
		size(Size), position(Postion), mesh(Mesh_), rotationAxis(RotationAxis), rotationAngle(Rangle), scale(Scale), 
		rotation(Rotation), model(LoadModelFromMesh(mesh)) {};

	void DrawandUpdate(float &roll, float &pitch, float &yaw)
	{
		rotation = MatrixMultiply(MatrixMultiply(MatrixRotateX(-roll), MatrixRotateY(-yaw)), MatrixRotateZ(-pitch));
		model.transform = rotation;
		DrawModelWires(model, position, 1.0f, BLACK);
		DrawModel(model, position, 1.0f, SKYBLUE );
		//DrawText(TextFormat("%d", roll), 0, 0, 20, RED);
		//DrawText(TextFormat("%d", pitch), 30, 0, 20, RED);
		//DrawText(TextFormat("%d", yaw), 60, 0, 20, RED);
	}
};

int main()
{
	float ax = 0.0f, ay = 0.0f, az = 0.0f;

	serialib serial;

	if (serial.openDevice("COM4", 115200) != 1)
	{
		std::cerr << "Falied to connect" << std::endl;
		return -1;
	}
	else
	{
		std::cout << "Connected" << std::endl;
	}

	const int ScreenWidth = 800, ScreenHeight = 600;
	InitWindow(ScreenWidth, ScreenHeight, "Cube_");
	SetTargetFPS(60);

	Vector3 Position = { 0.0f,0.0f,0.0f };
	Vector3 Size = { 2.0f, 2.0f, 2.0f };
	Vector3 RotationAxis = { 0.0f, 100.0f, 1.0f };
	Vector3 Scale = { 1.0f,1.0f,1.0f };

	float RotationAngle = 0.0f, SmoothX = 0.0f, SmoothY = 0.0f, SmoothZ = 0.0f;

	Mesh mesh = GenMeshCube(2.0f, 2.0f, 2.0f);
	Matrix rotation = { 0,0,0 };

    Camera3D cam = { 0.0f };
	cam.position = Vector3{ 10.0f,10.0f,10.0f };
	cam.projection = CAMERA_PERSPECTIVE;
	cam.target = Vector3{ 0.0f,0.0f,0.0f };
	cam.fovy = 30;
	cam.up = { 0.0f,1.0f,0.0f };

	CUBE cube(Position, Size, RotationAxis, Scale, mesh, rotation, RotationAngle);

	char c;
	std::string incoming = "";

	while (!WindowShouldClose())
	{
		while ((serial.readChar(&c, 2)) == 1)
		{
			
			//std::cout << "I'm here" << std::endl;
			if ((c == '\n'))
			{
				if (incoming.back() == '\r' && incoming.empty() == false)
				{
					incoming.pop_back();
				}
				int result = sscanf(incoming.c_str(), "%f,%f,%f", &ax, &ay, &az);
				incoming = "";
			}
			else
			{
				incoming = incoming + c;
			}
		} 
		std::cout << RAD2DEG * ax << " | " << RAD2DEG * ay << " | " << RAD2DEG * az << std::endl;
		SmoothX = 0.9 * SmoothX + 0.1 * ax;
		SmoothY = 0.9 * SmoothY + 0.1 * ay;
		SmoothZ = 0.9 * SmoothZ + 0.1 * az;
		BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode3D(cam);
		cube.DrawandUpdate(SmoothX, SmoothY, SmoothZ);
		EndMode3D();
		EndDrawing();
	}
	CloseWindow();
}
