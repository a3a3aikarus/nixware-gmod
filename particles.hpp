#pragma once
#include <vector>
#include "ISurface.hpp"
#include "pattern.hpp"

#define random_int(min, max) (rand() % (max - min + 1) + min)

float random_float(float min, float max)
{
	typedef float(*RandomFloat_t)(float, float);
	return ((RandomFloat_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat")) (min, max);
}

class particle
{
public:
	particle(float x, float y, float opacity, float x_velocity, float y_velocity) :
		x{ x },
		y{ y },
		opacity{ opacity },
		x_velocity{ x_velocity },
		y_velocity{ y_velocity } {}

	float x; // actually let it be a float.
	float y; // they need to move really slow
	float opacity;
	float x_velocity;
	float y_velocity;
};

class particle_network
{
public:
	particle_network(const int width, const int height, const int particles) :
		max_particles{ particles },
		screen_width{ width },
		screen_height{ height } {}

	~particle_network()
	{
		particles.clear();
	}

	void draw(const int _alpha)
	{
		alpha = _alpha;

		if (particles.empty()) // thanks mr resharper
		{
			for (auto i = 0; i < max_particles; i++)
				create_particle();
		}

		int x, y;
		globals::surface->surface_get_cursor_pos(x, y);

		auto mouse_particle = particle(x, y, 1, 0, 0);

		for (auto& particle : particles)
		{
			draw_particle(particle);
			update_position(particle);
			find_connections(particle, 200);
		}

		find_connections(mouse_particle, 200);
	}


	int get_width() const { return screen_width; }
	int get_height() const { return screen_height; }
private:
	int alpha = -1;

	void create_particle()
	{
		particles.push_back(
			particle(
				random_int(0, screen_width),
				random_int(0, screen_height),
				random_float(0.5f, 1),
				random_float(-0.1f, 0.1f),
				random_float(-0.1f, 0.1f)
			)
		);
	}

	void draw_particle(particle& particle) const
	{
		globals::surface->draw_set_color(Color(255, 255, 255, particle.opacity * alpha));
		globals::surface->draw_outlined_circle(particle.x, particle.y, 1, 8);
		globals::surface->draw_outlined_circle(particle.x, particle.y, 2, 8);
	}

	void draw_connection(particle& a, particle& b, const float opacity) const
	{
		globals::surface->draw_set_color(Color(255, 255, 255, opacity * alpha));
		globals::surface->draw_line(a.x, a.y, b.x, b.y);
	}

	void find_connections(particle& particle, const float max_distance)
	{
		for (auto& iter_particle : particles)
		{
			const auto distance = abs(particle.x - iter_particle.x) + abs(particle.y - iter_particle.y);
			if (distance < max_distance)
				draw_connection(particle, iter_particle, (max_distance - distance) / max_distance);
		}
	}

	void update_position(particle& particle) const
	{
		correct_velocity(particle);

		particle.x += particle.x_velocity;
		particle.y += particle.y_velocity;
	}

	void correct_velocity(particle& particle) const
	{
		if (particle.x + particle.x_velocity > screen_width || particle.x + particle.x_velocity < 0)
			particle.x_velocity = -particle.x_velocity;

		if (particle.y + particle.y_velocity > screen_height || particle.y + particle.y_velocity < 0)
			particle.y_velocity = -particle.y_velocity;
	}

	int max_particles = -1;
	int screen_width = -1;
	int screen_height = -1;
	std::vector<particle> particles;
};
