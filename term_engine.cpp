#define _TERM_ENGINE_IMPL
#include "term_engine.h"

void on_init()
{
}

void on_draw()
{
    std::stringstream s;
	s << SQ_CLEAR << SQ_SET_POS(12, 12);
	s << "Welcome to TE v0.0.1";
	s << SQ_SET_POS(4,4) << FRED << "sadfasdf" << CRESET;

    te::write_data(s);

}

void on_input(te::Context & c)
{
    char k = te::get_key();
	if(k == 'q')
		c.is_running = false;
}

void on_terminate()
{
}

int main()
{
    te::Context ctx;
    te::enable_rawmode();
    // te::get_win_size(ctx.w, ctx.h);

    ctx.w = 100;
    ctx.h = 100;
	ctx.is_running = true;
	te::set_size(ctx,100, 12);

    while(true)
    {
        on_draw();
        on_input(ctx);

        if(!ctx.is_running)
            break;
    }

    te::disable_rawmode();
    return 0;
}

