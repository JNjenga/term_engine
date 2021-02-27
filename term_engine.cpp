#define _TERM_ENGINE_IMPL
#include "term_engine.h"

extern te::Context ctx;

void te::on_init()
{
	ctx.is_running = true;
	te::set_size(100, 12);
}

void te::on_draw(std::stringstream & s)
{
	s << SQ_CLEAR << SQ_SET_POS(12, 12);
	s << "Welcome to TE v0.0.1";
	s << SQ_SET_POS(4,4) << FRED << "sadfasdf" << CRESET;
}

void te::on_input(const char c)
{
	if(c == 'q')
		ctx.is_running = false;
}

void te::on_terminate()
{
}
