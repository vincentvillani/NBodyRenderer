#include "include.h"

#include <iostream>
#include <vector>

#include <string>
#include <sstream>

#include <fstream>
#include <iostream>
#include <stdint.h>


#include "NBodyFileInput.h"


using namespace std;

vec2i _window_size;
bool _fullscreen = false;


enum APP_MODE
{
	PLAY_MODE,
	PAUSED_MODE,
	RENDERING_MOVIE_MODE
};

void floating_camera(Camera_Handle camera_handle)
{
	static vec3f angle;

	vec2i mouse_delta = lex::input::get_mouse_delta();

	double mouseX = 0;
	double mouseY = 0;

	float move_speed = 100.0f;

	//if (lex::input::key_down(GLFW_KEY_LEFT_ALT) && lex::input::mouse_down(GLFW_MOUSE_BUTTON_LEFT))
	{
		angle.x += mouse_delta.y * 0.1f;
		angle.y -= mouse_delta.x * 0.1f;
		Quaternion rotation = Quaternion(angle);
		camera_handle.set_camera_rotation(rotation);
	}

	if (lex::input::key_down(GLFW_KEY_W))
		camera_handle.set_camera_position(camera_handle.get_camera_position() - camera_handle.get_camera_rotation().get_z_axis() * Time::delta() * move_speed);

	if (lex::input::key_down(GLFW_KEY_S))
		camera_handle.set_camera_position(camera_handle.get_camera_position() + camera_handle.get_camera_rotation().get_z_axis() * Time::delta() * move_speed);

	if (lex::input::key_down(GLFW_KEY_A))
		camera_handle.set_camera_position(camera_handle.get_camera_position() - camera_handle.get_camera_rotation().get_x_axis() * Time::delta() * move_speed);

	if (lex::input::key_down(GLFW_KEY_D))
		camera_handle.set_camera_position(camera_handle.get_camera_position() + camera_handle.get_camera_rotation().get_x_axis() * Time::delta() * move_speed);
}



void take_screen_shot(string file_name)
{
	vec2i screen_size = lex::get_screen_size();
	
	
	ofstream file;
	file.open(file_name, ofstream::out | ofstream::binary);

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





void load_sim_file(FILE* sim_file, NBodyFileHeader* nbody_file_header, NBodyFrameBuffer & frame_buffer, Point_Mesh & point_mesh, unsigned int frame_index)
{
	FileInputReadFrame(nbody_file_header, frame_index, &frame_buffer, sim_file);


	point_mesh.clear();
	vec3f position;
	unsigned int index;

	float scale = 0.5f;

	int colorSize = 50;
	for(unsigned int i = 0; i < frame_buffer.particleNumber; ++i)
	{
		index = i * 3;


		position.x = frame_buffer.positions[index];
		position.y = frame_buffer.positions[index + 1];
		position.z = frame_buffer.positions[index + 2];

		point_mesh.add_point(position * scale, Color( 1,1,1), 1.0f);

		if(i == 0)
		{
			//printf(position.to_string().c_str());
		}
	}

	point_mesh.update_mesh();

}


void render_animation(FILE* sim_file, NBodyFileHeader* nbody_file_header, NBodyFrameBuffer & frame_buffer, Point_Mesh & point_mesh, Camera_Handle camera)
{

	char out_file_buffer[100];

	for(unsigned int i = 0; i < 10; ++i)
	{
		lex::update();


		load_sim_file(sim_file, nbody_file_header, frame_buffer, point_mesh, i);

		lex::update_transforms();

		camera.draw();

		point_mesh.draw(camera.get_projection_matrix(), camera.get_view_matrix());

		sprintf(out_file_buffer, "output/%d.tga", i);

		take_screen_shot(out_file_buffer);

		lex::swap_buffers();
	}
}


int main(int argc, char *argv[])
{

	Resource_Management::set_engine_data_path("C:/msys64/home/Trent/lex_engine/data/");
	//Resource_Management::set_data_path("C:/msys64/home/Trent/cave/data/");


	unsigned int width = 1280;
	unsigned int height = 720;

	if(argc == 3)
	{
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}

	_window_size = vec2i(width, height);
	lex::initilise(_window_size, lex::CORE_PROFILE);


	// Create a camera
	Camera_Handle main_camera = lex::camera::create();
	main_camera.set_perspective(65.0f, 0.01f, 10000.0f);
	main_camera.set_camera_zoom(0);

	main_camera.set_clear_color(Color(0.2f, 0.2f, 0.2f));
	main_camera.set_clear_flags(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Create a canvas

	Canvas_2d canvas;
	canvas.init();


	bool quit = false;

	Point_Mesh point_mesh;
	point_mesh.init();


	unsigned int frame_since_start = 0;
	int last_frame = frame_since_start;

	unsigned int current_frame = 0;



	APP_MODE app_mode = PAUSED_MODE;



	FILE* input_sim_file = fopen("OutputFile.sim", "rb");

	if(input_sim_file == NULL)
	{
		lex::close();
		return EXIT_SUCCESS;
	}

	NBodyFileHeader* nbody_file_header = FileInputReadHeader(input_sim_file);
	NBodyFrameBuffer frame_buffer(0, nbody_file_header->numberOfParticles);

	load_sim_file(input_sim_file, nbody_file_header, frame_buffer, point_mesh, current_frame);

	while (!quit)
	{
		lex::update();

		if(lex::input::key_hit(GLFW_KEY_M))
		{
			render_animation(input_sim_file, nbody_file_header, frame_buffer, point_mesh, main_camera);

		}

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


		if(app_mode != RENDERING_MOVIE_MODE)
		{

			int new_frame =  (int)(Time::get_milliseconds() / 16.66f) ;



			if(lex::input::key_hit(GLFW_KEY_Z))
			{
				app_mode = PAUSED_MODE;
				current_frame--;

				if(current_frame > nbody_file_header->numberOfFrames)
					current_frame = nbody_file_header->numberOfFrames - 1;

				load_sim_file(input_sim_file, nbody_file_header, frame_buffer, point_mesh, current_frame);
			}
			else if(lex::input::key_hit(GLFW_KEY_X))
			{
				app_mode = PAUSED_MODE;
				current_frame++;

				if(current_frame >= nbody_file_header->numberOfFrames)
					current_frame = 0;

				load_sim_file(input_sim_file, nbody_file_header, frame_buffer, point_mesh, current_frame);
			}
			else if(lex::input::key_hit(GLFW_KEY_SPACE))
			{
				if(app_mode == PLAY_MODE)
					app_mode = PAUSED_MODE;
				else if(app_mode == PAUSED_MODE)
					app_mode = PLAY_MODE;
			}


			if(app_mode == PLAY_MODE && last_frame != new_frame)
			{
				current_frame++;

				if(current_frame >= nbody_file_header->numberOfFrames)
					current_frame = 0;

				load_sim_file(input_sim_file, nbody_file_header, frame_buffer, point_mesh, current_frame);

				last_frame = new_frame;
			}



			floating_camera(main_camera);

		}


		ostringstream string_stream;
		string_stream << Time::get_fps();

		canvas.draw_text(string_stream.str(), vec2f(0, 20));

		string_stream.str(std::string());

		string_stream << "Frame Number: " << current_frame;
		canvas.draw_text(string_stream.str(), vec2f(600, 20));


		main_camera.draw_line(vec3f(-1, -1, -1), vec3f(0, -1, -1), Color(1, 0, 0));
		main_camera.draw_line(vec3f(-1, -1, -1), vec3f(-1, 0, -1), Color(0, 1, 0));
		main_camera.draw_line(vec3f(-1, -1, -1), vec3f(-1, -1, 0), Color(0, 0, 1));


		lex::update_transforms();

		main_camera.draw();

		canvas.draw(lex::get_screen_orthographic_projection(), mat4::identity());

		point_mesh.draw(main_camera.get_projection_matrix(), main_camera.get_view_matrix());

		



		lex::swap_buffers();

	}



	fclose(input_sim_file);



	lex::close();

	return EXIT_SUCCESS;
}
