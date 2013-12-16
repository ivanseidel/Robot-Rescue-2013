
#define NUM_PARTICLES 100

#define MAP_SIZE_X 30
#define MAP_SIZE_Y 15

#define WHEEL_DISTANCE 0.16
#define WHEEL_DIAMETER 0.055

struct State
{
	double x;
	double y;
	double theta;
	double timestamp;
};

struct Control
{
	double vl, vr;
	double timestamp;
};

typedef struct State State;
typedef struct Control Control;

class CatabrigaActivity: public Activity
{
public:

	State particles[NUM_PARTICLES];
	unsigned char map[MAP_SIZE_X][MAP_SIZE_Y];

	CatabrigaActivity(){
		Activity::Activity();
		ActivityName = "Catabriga Program";

		initParticles();
		initMap();
	}

	void initParticles()
	{
		State zero;
		zero.x = 150;
		zero.y = 75;
		zero.theta = 0;
		zero.timestamp = micros();

		for(int i=0; i<NUM_PARTICLES; i++)
		{
			particles[i] = zero;
		}
	}

	void initMap()
	{
		for(int i=0; i<MAP_SIZE_X; i++)
		{
			for(int j=0; j<MAP_SIZE_Y; j++)
			{
				map[i][j] = 0;
			}
		}
	}

	void onResume(){
	
	}

	void onStop(){

	}


	void drawMap()
	{
		STAGE.render();
		ArdUI::LCD->setColor(color(255,255,255));
		ArdUI::LCD->drawRect( 100, 80, 700, 380);

		for(int i=0; i<MAP_SIZE_X; i++)
		{
			for(int j=0; j<MAP_SIZE_Y; j++)
			{
				unsigned char prob = map[i][j];

				if(prob == 0)
				{
					ArdUI::LCD->setColor(color(255,0,0));
				}
				else
				{
					ArdUI::LCD->setColor(color(prob, prob, prob));
				}

				ArdUI::LCD->fillRect(2*i+100, 2*j+80, 2*(i+1)+100, 2*(j+1)+80);
			}
		}

		ArdUI::LCD->setColor(color(0,0,255));
		for(int i=0; i<NUM_PARTICLES; i++)
		{
			int x = (int)(2*particles[i].x + 0.5);
			int y = (int)(2*particles[i].y + 0.5);

			ArdUI::LCD->fillRect(x+100, y+80, x+100+4, y+80+4);
		}
	}

	State predictParticle(State particle, Control control)
	{
		// http://chess.eecs.berkeley.edu/eecs149/documentation/differentialDrive.pdf

		double dt = control.timestamp - particle.timestamp;
		double vRight = control.vr;
		double vLeft = control.vl;

		//vRight += Gaussian(0, 0.002).random();
		//vLeft += Gaussian(0, 0.002).random();

		double R = (WHEEL_DISTANCE / 2.0) * (vRight + vLeft) / (vRight - vLeft);
		double w = (vRight - vLeft) / WHEEL_DISTANCE;

		double x = particle.x;
		double y = particle.y;
		double theta = particle.theta;

		double ICCx = x - R * sin(theta);
		double ICCy = y + R * cos(theta);

		State newParticle;
		newParticle.x = cos(w * dt) * (x - ICCx) - sin(w * dt) * (y - ICCy) + ICCx;
		newParticle.y = sin(w * dt) * (x - ICCx) + cos(w * dt) * (y - ICCy) + ICCy;
		newParticle.theta = theta + w * dt;

		return newParticle;
	}

	void prediction(Control control)
	{
		for(int i=0; i<NUM_PARTICLES; i++)
		{
			particles[i] = predictParticle(particles[i], control);
		}
	}

	void correction()
	{

	}

	Control createControl()
	{
		static long lastM0 = 0;
		static long lastM1 = 0;
		static long lastM2 = 0;
		static long lastM3 = 0;
		static long lastTimestamp = 0;

		if(lastTimestamp == 0){
			lastTimestamp = micros();
			delay(40);
		}

		MD->readTicks();
		long timestamp = micros();

		long m0 = MD->getTicks((Motor)0);
		long m1 = MD->getTicks((Motor)1);
		long m2 = MD->getTicks((Motor)2);
		long m3 = MD->getTicks((Motor)3);

		long d0 = m0 - lastM0;
		long d1 = m1 - lastM1;
		long d2 = m2 - lastM2;
		long d3 = m3 - lastM3;
		long dt = timestamp - lastTimestamp;

		double vr = (d0 + d1) / (2.0 * dt); // ticks por microsegundo
		double vl = (d2 + d3) / (2.0 * dt);

		vr = (vr / 1796) / 1000000; // rotacao por segundo
		vl = (vl / 1796) / 1000000;

		vr = 3.141596 * WHEEL_DIAMETER * vr; // metros por segundo
		vl = 3.141596 * WHEEL_DIAMETER * vl; // metros por segundo

		lastM0 = m0;
		lastM1 = m1;
		lastM2 = m2;
		lastM3 = m3;
		lastTimestamp = timestamp;

		Control control;
		control.vr = vr;
		control.vl = vl;
		control.timestamp = timestamp;

		return control;
	}

	void onLoop(){

		Control control = createControl();
		prediction(control);
		correction();

		drawMap();
		delay(300);
	}

};