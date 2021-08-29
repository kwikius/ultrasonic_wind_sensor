

#include "web_pages.h"
namespace {

constexpr char compass_js[] = R"(
function vect(x,y)
{
   this.x = x;
   this.y = y;
}

function toRad(x)
{
  return x * (Math.PI / 180);
}

function clearCanvas(ctx) 
{
	ctx.clearRect(0, 0, 200, 200);
}

function circle(ctx,centre,radius)
{
   ctx.beginPath();
   ctx.arc(centre.x,centre.y,radius,0,2* Math.PI);
   ctx.stroke();
}

function line(ctx, from , to)
{
   ctx.moveTo(from.x,from.y);
   ctx.lineTo(to.x,to.y);
   ctx.stroke();
}

function arrow(ctx, pos, magnitude, direction)
{
	const p1 = new vect(0, - magnitude);
   const arrow_hd_width = 20;
   const arrow_hd_length = 60;
   const p2 = new vect(-arrow_hd_width,-arrow_hd_length);
   const p3 = new vect( arrow_hd_width,-arrow_hd_length);
   const origin = new vect(0,0);
	ctx.save();
	ctx.translate(pos.x, pos.y);
	ctx.rotate(toRad(direction));
   line(ctx,origin,p1);
   line(ctx,origin,p2);
   line(ctx,origin,p3);
	ctx.restore(); 
}

function draw_wind(direction_deg, speed_knots)
{
   const canvas = document.getElementById("theCanvas");
    
   const canvas_width = canvas.width;
   const canvas_height = canvas.height;
   const canvas_side = Math.min(canvas_width,canvas_height);
   const border = 20;
   const rect_side = canvas_side - border*2;
   const context = canvas.getContext("2d");
   
   const max_knots = 20;
   const knots_scale = (rect_side/2)/max_knots;
   const centre = new vect(canvas_width/2,canvas_height/2);

   context.clearRect(0,0, canvas_width, canvas_height);
   circle(context,centre,rect_side/2 - border);
   arrow(context,centre,knots_scale*speed_knots,direction_deg);
}
)";

} // ~namespace

const char* get_web_page_compass_js()
{
   return compass_js;
}

//----------------------------------------------

