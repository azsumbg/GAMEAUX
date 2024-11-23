#include "pch.h"
#include "GameAux.h"

//RANDOMIZER *********************

dll::RANDOMIZER::RANDOMIZER()
{
	vSeeds.clear();
	for (int i = 0; i < 650; i++) vSeeds.push_back(rDevice());
	sequencer = new std::seed_seq(vSeeds.begin(), vSeeds.end());
	twister = new std::mt19937(*sequencer);
}
dll::RANDOMIZER::~RANDOMIZER()
{
	delete sequencer;
	delete twister;
}
int dll::RANDOMIZER::operator()(int min, int max)
{
	std::uniform_int_distribution distrib{ min, max };
	return distrib(*twister);
}

///////////////////////////////////

//PROTON ************************

dll::PROTON::PROTON(float _x, float _y, float _width, float _height)
{
	x = _x;
	y = _y;
	width = _width;
	height = _height;
	ex = x + width;
	ey = y + height;
}

float dll::PROTON::GetWidth() const
{
	return width;
}
float dll::PROTON::GetHeight() const
{
	return height;
}

void dll::PROTON::SetWidth(float new_width)
{
	width = new_width;
	ex = x + width;
}
void dll::PROTON::SetHeight(float new_height)
{
	height = new_height;
	ey = y + height;
}

void dll::PROTON::SetEdges()
{
	ex = x + width;
	ey = y + height;
}
void dll::PROTON::NewDims(float new_width, float new_height)
{
	width = new_width;
	height = new_height;
	ey = y + height;
	ex = x + width;
}

bool dll::PROTON::GetFlag(unsigned char which_flag) const
{
	return type_flag & which_flag;
}
void dll::PROTON::SetFlag(unsigned char which_flag)
{
	type_flag = 0;
	type_flag |= which_flag;
}
void dll::PROTON::NullFlag(unsigned char which_flag)
{
	type_flag &= ~which_flag;
}
unsigned char dll::PROTON::GetAllFlags() const
{
	return type_flag;
}

//////////////////////////////////


//PROT_CONTAINER ****************

dll::PROT_CONTAINER::PROT_CONTAINER(size_t lenght) :size{ lenght }, mBase_ptr{ new dll::PROTON[lenght] }
{
	next_push_position = 0;
}
dll::PROT_CONTAINER::~PROT_CONTAINER()
{
	delete[] mBase_ptr;
}

void dll::PROT_CONTAINER::push_back(PROTON& element)
{
	if (next_push_position + 1 > size)return;
	*(mBase_ptr + next_push_position) = element;
	++next_push_position;
}
void dll::PROT_CONTAINER::push_front(PROTON& element)
{
	*mBase_ptr = element;
}

dll::PROTON& dll::PROT_CONTAINER::front()const
{
	return *mBase_ptr;
}
dll::PROTON& dll::PROT_CONTAINER::back()const
{
	return *(mBase_ptr + (next_push_position - 1));
}

size_t dll::PROT_CONTAINER::capacity() const
{
	return size;
}

dll::PROTON& dll::PROT_CONTAINER::operator[] (size_t position)
{
	PROTON* InvalidProton = new PROTON();
	if (position >= size)return *InvalidProton;
	return *(mBase_ptr + position);
}
void dll::PROT_CONTAINER::operator() (size_t position, dll::PROTON& element)
{
	if (position >= size)return;
	*(mBase_ptr + position) = element;
}

float dll::PROT_CONTAINER::distance(PROTON first, PROTON second) const
{
	
	if ((abs)(second.y - first.y) == 0)return abs(second.x - first.x);
	if ((abs)(second.x - first.x) == 0)return abs(second.y - first.y);

	return static_cast<float>(sqrt(pow(second.x - first.x, 2) + pow(second.y - first.y, 2)));
}
void dll::PROT_CONTAINER::distance_sort(dll::PROTON base_point)
{
	if (size < 2)return;

	bool nearest_found = false;
	while (!nearest_found)
	{
		nearest_found = true;

		for (int i = 0; i < size - 1; i++)
		{
			float distance1 = distance(base_point, *(mBase_ptr + i));
			float distance2 = distance(base_point, *(mBase_ptr + (i + 1)));

			if (distance2 < distance1)
			{
				PROTON temp = *(mBase_ptr + i);
				*(mBase_ptr + i) = *(mBase_ptr + (i + 1));
				*(mBase_ptr + (i + 1)) = temp;
				nearest_found = false;
			}
		}
	}
	
}

//////////////////////////////////

//BASE_CREATURE *****************

dll::BASE_CREATURE::BASE_CREATURE(unsigned char type, float startX, float startY) :PROTON(startX, startY)
{
	type_flag = type;

	switch (type_flag)
	{
	case hero_flag:
		NewDims(50.0f, 60.0f);
		strenght = 20;
		speed = 2.0f;
		max_frames = 8;
		frame_delay = 9;
		attack_delay = 20;
		lifes = 200;
		break;

	case zombie1_flag:
		NewDims(50.0f, 70.0f);
		strenght = 5;
		speed = 0.7f;
		max_frames = 2;
		frame_delay = 35;
		attack_delay = 100;
		lifes = 50;
		break;

	case zombie2_flag:
		NewDims(59.0f, 70.0f);
		strenght = 8;
		speed = 0.5f;
		max_frames = 11;
		frame_delay = 6;
		attack_delay = 120;
		lifes = 80;
		break;

	case zombie3_flag:
		NewDims(40.0f, 60.0f);
		strenght = 4;
		speed = 0.7f;
		max_frames = 7;
		frame_delay = 10;
		attack_delay = 110;
		lifes = 40;
		break;

	case bullet_flag:
		NewDims(35.0f, 35.0f);
		speed = 10.0f;
		max_frames = 32;
		frame_delay = 2;
		break;
	}
}
void dll::BASE_CREATURE::SetPathInfo(float _end_x, float _end_y)
{
	hor_line = false;
	ver_line = false;

	move_x = x;
	move_y = y;
	move_ex = _end_x;
	move_ey = _end_y;

	if (move_ex - move_x == 0)
	{
		ver_line = true;
		return;
	}
	if (move_ey - move_y == 0)
	{
		hor_line = true;
		return;
	}

	slope = (move_ey - move_y) / (move_ex - move_x);
	intercept = move_y - move_x * slope;
}
int dll::BASE_CREATURE::GetFrame()
{
	--frame_delay;
	if (frame_delay <= 0)
	{
		switch (type_flag)
		{
		case hero_flag:
			frame_delay = 9;
			break;

		case zombie1_flag:
			frame_delay = 35;
			break;

		case zombie2_flag:
			frame_delay = 6;
			break;

		case zombie3_flag:
			frame_delay = 10;
			break;

		case bullet_flag:
			frame_delay = 2;
			break;
		}
		
		++current_frame;
		if (current_frame > max_frames)current_frame = 0;
	}

	return current_frame;
}
int dll::BASE_CREATURE::Attack()
{
	attack_delay--;
	if (attack_delay <= 0)
	{
		switch (type_flag)
		{
		case hero_flag:
			attack_delay = 20;
			break;

		case zombie1_flag:
			attack_delay = 100;
			break;

		case zombie2_flag:
			attack_delay = 120;
			break;

		case zombie3_flag:
			frame_delay = 10;
			break;
		}
		return strenght;
	}
	return 0;
}
void dll::BASE_CREATURE::ObstacleDetour(PROTON& Obstacle)
{
	obstacle_flags = 0b00000000;
	
	if (x > Obstacle.x && x <= Obstacle.ex)obstacle_flags |= obstacle_left;
	if (ex >= Obstacle.x && ex < Obstacle.ex)obstacle_flags |= obstacle_right;
	
	if (y < Obstacle.ey && y >= Obstacle.y)obstacle_flags |= obstacle_up;
	if (ex >= Obstacle.y && ey < Obstacle.ey)obstacle_flags |= obstacle_down;

	if (obstacle_flags)
	{
		switch (obstacle_flags)
		{
		case obstacle_down:
			Move(true, x, sky, speed);
			break;

		case obstacle_up:
			Move(true, x, ground, speed);
			break;

		case obstacle_left:
			Move(true, scr_width, y, speed);
			break;

		case obstacle_right:
			Move(true, 0, y, speed);
			break;

		case obstacle_up_right:
			Move(true, 0, ground, speed);
			break;

		case obstacle_up_left:
			Move(true, scr_width, ground, speed);
			break;

		case obstacle_down_right:
			Move(true, 0, sky, speed);
			break;

		case obstacle_down_left:
			Move(true, scr_width, sky, speed);
			break;
		}
	}
}

//////////////////////////////////

// HERO CLASS ********************

class HERO :public dll::BASE_CREATURE
{
	public:

		HERO(float first_x, float first_y) :BASE_CREATURE(hero_flag, first_x, first_y)
		{
			if (first_x >= scr_width / 2)dir = dirs::left;
			else dir = dirs::right;
		}
		void Release() override
		{
			delete this;
		}
		void Move(bool need_new_path, float dest_x, float dest_y, float gear) override
		{
			float now_speed = speed / 5 + gear / 10;

			if (need_new_path)SetPathInfo(dest_x, dest_y);

			if (hor_line)
			{
				if (move_ex < move_x && x - now_speed >= 0)
				{
					dir = dirs::left;
					x -= now_speed;
					SetEdges();
					return;
				}
				else if (move_ex > move_x && ex + now_speed <= scr_width)
				{
					dir = dirs::right;
					x += now_speed;
					SetEdges();
					return;
				}
				else return;
			}
			if (ver_line)
			{
				if (move_ey < move_y && y - now_speed >= sky)
				{
					y -= now_speed;
					SetEdges();
					return;
				}
				else if (move_ey > move_y && ey + now_speed <= ground)
				{
					y += now_speed;
					SetEdges();
					return;
				}
				else return;
			}

			if (move_ex < move_x && x - now_speed >= 0)  // MOVE LEFT
			{
				dir = dirs::left;

				if (move_ey < move_y && y - now_speed >= sky) // UP LEFT
				{
					x -= now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
				else if (move_ey > move_y && ey + now_speed <= ground) // DOWN LEFT 
				{
					x -= now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
				else return;
			}

			if (move_ex > move_x && ex + now_speed <= scr_width)  // MOVE RIGHT
			{
				dir = dirs::right;

				if (move_ey < move_y && y - now_speed >= sky) // UP RIGHT
				{
					x += now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
				else if (move_ey > move_y && ey + now_speed <= ground) // DOWN RIGHT 
				{
					x += now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
				else return;
			}
		}
};

// ZOMBIE CLASS

class ZOMBIE :public dll::BASE_CREATURE
{
	public:
		ZOMBIE(unsigned char what, float initial_x, float initial_y) :BASE_CREATURE(what, initial_x, initial_y)
		{
			dir = dirs::left;
		}

		void Release() override
		{
			delete this;
		}
		void Move(bool need_new_path, float dest_x, float dest_y, float gear) override
		{
			float now_speed = speed / 5 + gear / 15;

			if (need_new_path)SetPathInfo(dest_x, dest_y);

			if (hor_line)
			{
				if (move_ex < move_x && x - now_speed >= 0)
				{
					dir = dirs::left;
					x -= now_speed;
					SetEdges();
					return;
				}
				else if (move_ex > move_x && ex + now_speed <= scr_width)
				{
					dir = dirs::right;
					x += now_speed;
					SetEdges();
					return;
				}
				else return;
			}
			if (ver_line)
			{
				if (move_ey < move_y && y - now_speed >= sky)
				{
					y -= now_speed;
					SetEdges();
					return;
				}
				else if (move_ey > move_y && ey + now_speed <= ground)
				{
					y += now_speed;
					SetEdges();
					return;
				}
				else return;
			}

			if (move_ex < move_x && x - now_speed >= 0)  // MOVE LEFT
			{
				dir = dirs::left;

				if (move_ey < move_y && y - now_speed >= sky) // UP LEFT
				{
					x -= now_speed;
					y = x * slope + intercept;
					SetEdges();
				}
				else if (move_ey > move_y && ey + now_speed <= ground) // DOWN LEFT 
				{
					x -= now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
			}

			if (move_ex > move_x && ex + now_speed <= scr_width)  // MOVE RIGHT
			{
				dir = dirs::right;

				if (move_ey < move_y && y - now_speed >= sky) // UP RIGHT
				{
					x += now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
				else if (move_ey > move_y && ey + now_speed <= ground) // DOWN RIGHT 
				{
					x += now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
				else return;
			}
		
		}
};

/////////////////////////

class BULLET :public dll::BASE_CREATURE
{
	public:
		BULLET(float _first_x, float _first_y, float _end_x, float _end_y, int my_strenght) :BASE_CREATURE(bullet_flag, 
			_first_x, _first_y)
		{
			SetPathInfo(_end_x, _end_y);
			strenght = my_strenght;
		}

		void Release() override
		{
			delete this;
		}

		void Move(bool need_new_path, float dest_x, float dest_y, float gear) override
		{
			float now_speed = speed + gear / 10;

			if (hor_line)
			{
				if (move_ex < move_x && x - now_speed >= 0)
				{
					dir = dirs::left;
					x -= now_speed;
					SetEdges();
					return;
				}
				else if (move_ex > move_x && ex + now_speed <= scr_width)
				{
					dir = dirs::right;
					x += now_speed;
					SetEdges();
					return;
				}
				else return;
			}
			if (ver_line)
			{
				if (move_ey < move_y && y - now_speed >= sky)
				{
					y -= now_speed;
					SetEdges();
					return;
				}
				else if (move_ey > move_y && ey + now_speed <= ground)
				{
					y += now_speed;
					SetEdges();
					return;
				}
				else return;
			}

			if (move_ex < move_x && x - now_speed >= 0)  // MOVE LEFT
			{
				dir = dirs::left;

				if (move_ey < move_y && ey >= sky) // UP LEFT
				{
					x -= now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
				else if (move_ey > move_y && y <= ground) // DOWN LEFT 
				{
					x -= now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
				else return;
			}

			if (move_ex > move_x && ex <= scr_width)  // MOVE RIGHT
			{
				dir = dirs::right;

				if (move_ey < move_y && ey >= sky) // UP RIGHT
				{
					x += now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
				else if (move_ey > move_y && y + now_speed <= ground) // DOWN RIGHT 
				{
					x += now_speed;
					y = x * slope + intercept;
					SetEdges();
					return;
				}
				else return;
			}
		}
};

// FACTORY 

dll::creature_ptr dll::CreatureFactory(unsigned char what, float initial_x, float initial_y, 
	float bull_end_x, float bull_end_y, int bullet_strenght)
{
	creature_ptr ret = nullptr;

	switch (what)
	{
	case hero_flag:
		ret = new HERO(initial_x, initial_y);
		break;

	case zombie1_flag:
		ret = new ZOMBIE(what, initial_x, initial_y);
		break;

	case zombie2_flag:
		ret = new ZOMBIE(what, initial_x, initial_y);
		break;

	case zombie3_flag:
		ret = new ZOMBIE(what, initial_x, initial_y);
		break;

	case bullet_flag:
		ret = new BULLET(initial_x, initial_y, bull_end_x, bull_end_y, bullet_strenght);
		break;
	}

	return ret;
}