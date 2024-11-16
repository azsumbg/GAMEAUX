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

	return sqrt(pow(second.x - first.x, 2) + pow(second.y - first.y, 2));
}
void dll::PROT_CONTAINER::distance_sort(dll::PROTON base_point)
{
	if (size < 2)return;

	bool nearest_found = false;
	while (!nearest_found)
	{
		nearest_found = true;

		for (int i = 0; i < size; i++)
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
		max_frames = 8;
		frame_delay = 9;
		attack_delay = 20;
		lifes = 200;
		break;

	case zombie1_flag:
		NewDims(50.0f, 70.0f);
		strenght = 5;
		max_frames = 2;
		frame_delay = 35;
		attack_delay = 100;
		lifes = 50;
		break;

	case zombie2_flag:
		NewDims(59.0f, 70.0f);
		strenght = 8;
		max_frames = 11;
		frame_delay = 6;
		attack_delay = 120;
		lifes = 80;
		break;

	case zombie3_flag:
		NewDims(40.0f, 60.0f);
		strenght = 4;
		max_frames = 7;
		frame_delay = 10;
		attack_delay = 110;
		lifes = 40;
		break;
	}
}
void dll::BASE_CREATURE::Release()
{
	delete this;
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

	slope = (move_ey - move_y) / (move_ey - move_y);
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
		}
		
		++current_frame;
		if (current_frame > max_frames)current_frame = 0;
	}

	return current_frame;
}
