#include "include.h"

#include <iostream>
#include <vector>

#include <string>
#include <sstream>

#include <fstream>
#include <iostream>
#include <stdint.h>

using namespace std;

vec2i _window_size;
bool _fullscreen = false;


void floating_camera(Camera_Handle camera_handle)
{
	static vec3f angle;

	vec2i mouse_delta = lex::input::get_mouse_delta();

	double mouseX = 0;
	double mouseY = 0;

	//if (lex::input::key_down(GLFW_KEY_LEFT_ALT) && lex::input::mouse_down(GLFW_MOUSE_BUTTON_LEFT))
	{
		angle.x += mouse_delta.y * 0.1f;
		angle.y -= mouse_delta.x * 0.1f;
		Quaternion rotation = Quaternion(angle);
		camera_handle.set_camera_rotation(rotation);
	}

	if (lex::input::key_down(GLFW_KEY_W))
		camera_handle.set_camera_position(camera_handle.get_camera_position() - camera_handle.get_camera_rotation().get_z_axis() * Time::delta() * 10);

	if (lex::input::key_down(GLFW_KEY_S))
		camera_handle.set_camera_position(camera_handle.get_camera_position() + camera_handle.get_camera_rotation().get_z_axis() * Time::delta() * 10);

	if (lex::input::key_down(GLFW_KEY_A))
		camera_handle.set_camera_position(camera_handle.get_camera_position() - camera_handle.get_camera_rotation().get_x_axis() * Time::delta() * 10);

	if (lex::input::key_down(GLFW_KEY_D))
		camera_handle.set_camera_position(camera_handle.get_camera_position() + camera_handle.get_camera_rotation().get_x_axis() * Time::delta() * 10);
}



void take_screen_shot()
{
	vec2i screen_size = lex::get_screen_size();
	
	
	ofstream file;
	file.open("screenshot.tga", ofstream::out | ofstream::binary);

	if (file.is_open() == false)
		return;
	
	unsigned int image_data_size = 3 * screen_size.x * screen_size.y;
	char * image_data = new(nothrow) char[image_data_size];

	if(image_data == NULL)
	{
		file.close();
		return;
	}

	memset(image_data, 255, image_data_size);

	glFinish();
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, screen_size.x, screen_size.y, GL_BGR, GL_UNSIGNED_BYTE, image_data);

	char tga_header[18];

	for(unsigned int i = 0; i < 18; ++i)
	{
		tga_header[i] = 0;
	}


	tga_header[2] = 2;


	uint16_t width = screen_size.x;
	uint16_t height = screen_size.y;

	memcpy(&tga_header[12], &width, 2);
	memcpy(&tga_header[14], &height, 2);



	tga_header[16] = 24;

	file.write(tga_header, 18);

	file.write(image_data, image_data_size);

	file.close();

	delete[] image_data;
}




int main(int argc, char *argv[])
{
	//UNUSED(argc);
	//UNUSED(argv);


	Resource_Management::set_engine_data_path("C:/msys64/home/Trent/lex_engine/data/");
	//Resource_Management::set_data_path("C:/msys64/home/Trent/cave/data/");


	_window_size = vec2i(1280, 720);
	lex::initilise(_window_size, lex::CORE_PROFILE);


	// Create a camera
	Camera_Handle main_camera = lex::camera::create();
	main_camera.set_perspective(65.0f, 0.01f, 1000.0f);
	main_camera.set_camera_zoom(0);

	main_camera.set_clear_color(Color(0.2f, 0.2f, 0.2f));
	main_camera.set_clear_flags(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Create a canvas

	Canvas_2d canvas;
	canvas.init();


	bool quit = false;

	vector<vec3f> point_list;

	Point_Mesh point_mesh;

	int point_count = 5000000;

	point_mesh.init();

	vec3f point;
	for (unsigned int i = 0; i < point_count; ++i)
	{
		
		point.x = (rand() % 1000) / 1000.0f * 100;
		point.y = (rand() % 1000) / 1000.0f * 100;
		point.z = (rand() % 1000) / 1000.0f * 100;

		point_mesh.add_point(point, Color(1.0f, point.y / 100.0f, point.z / 100.0f), 3.0f);
	}


	point_mesh.update_mesh();

	while (!quit)
	{

		lex::update();

		if (lex::input::key_down(GLFW_KEY_ESCAPE))
			break;

		if (lex::input::key_down(GLFW_KEY_RIGHT_ALT) && lex::input::key_hit(GLFW_KEY_ENTER))
		{
			_fullscreen = !_fullscreen;

			if (_fullscreen == true)
				lex::screen::set_windowed_fullscreen();
			else
				lex::screen::set_windowed(200, 200);
		}


		floating_camera(main_camera);


		ostringstream string_stream;
		string_stream << Time::get_fps();

		canvas.draw_text(string_stream.str(), vec2f(0, 20));



		main_camera.draw_line(vec3f(-1, -1, -1), vec3f(0, -1, -1), Color(1, 0, 0));
		main_camera.draw_line(vec3f(-1, -1, -1), vec3f(-1, 0, -1), Color(0, 1, 0));
		main_camera.draw_line(vec3f(-1, -1, -1), vec3f(-1, -1, 0), Color(0, 0, 1));

		//point_mesh.update_mesh();

		lex::update_transforms();

		main_camera.draw();

		canvas.draw(lex::get_screen_orthographic_projection(), mat4::identity());

		point_mesh.draw(main_camera.get_projection_matrix(), main_camera.get_view_matrix());

		

		if (lex::input::key_hit(GLFW_KEY_A))
			take_screen_shot();

		lex::swap_buffers();

	}





	lex::close();

	return EXIT_SUCCESS;
}
