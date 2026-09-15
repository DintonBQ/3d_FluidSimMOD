#include "Header.h"
Point_3d LightPosition{ 0,0,0 };
double LightRadius = 35;
double ParticleRadius = 10;
double FromEyeToLight = 440; //1500 це z=0

std::vector<Point_3d> ParticlePositions;
std::vector<Point_3d> ParticleSpeed3D;
static bool dinOnce = false;
int ParticleCount = 120;




static double InteractionForce(double distance, double minDist, double maxDist, double repulseStrength, double attractStrength) {
	if(distance < minDist) {
		return -repulseStrength * (minDist - distance) / minDist; //чим ближче - тим сильніше відштовхує
	} else if(distance > maxDist) {
		double f = attractStrength * (distance - maxDist) / maxDist;
		double capMax = attractStrength * 4; //щоб частинки не розганялись до нескінченності
		if(f > capMax) f = capMax;
		return f; //чим далі - тим сильніше притягує(з обмеженням)
	}
	return 0; //в комфортній зоні - без сили
}

void UpdateFluid(double simSpeed) {
	if(!dinOnce) { dinOnce = true;
		ParticlePositions.clear();
		ParticleSpeed3D.clear();
		for(int i = 0; i < ParticleCount; i++) {
			double x = (double)(rand() % 200/*0..199*/ - 100); //-100..99
			double y = (double)(rand() % 200 - 100);
			double z =  (-From + FromEyeToLight/*там де Light*/) + (double)(rand() % 200 - 100);
			ParticlePositions.push_back(Point_3d{ x, y, z });
			ParticleSpeed3D.push_back(Point_3d{ 0,0,0 });
		}
	}

//PARTICLE <-> PARTICLE
	double minDistance = ParticleRadius; //якщо 2 радіуса то щільно
	double maxDistance = ParticleRadius * 2.0;

	std::vector<Point_3d> forces(ParticleCount, Point_3d{ 0,0,0 });
	for(int i = 0; i < ParticleCount; i++) {
		for(int j = i + 1; j < ParticleCount; j++) {
			Point_3d d{ ParticlePositions[j].x - ParticlePositions[i].x,
						ParticlePositions[j].y - ParticlePositions[i].y,
						ParticlePositions[j].z - ParticlePositions[i].z };
			double distance;
			Point_3d dir = normalizeVector(d, distance);

			double forceValue = InteractionForce(distance, minDistance, maxDistance, 900.0, 40.0);
			//particle1 forces
			forces[i].x += dir.x * forceValue;
			forces[i].y += dir.y * forceValue;
			forces[i].z += dir.z * forceValue;
			
			//particle2 forces
			forces[j].x -= dir.x * forceValue;
			forces[j].y -= dir.y * forceValue;
			forces[j].z -= dir.z * forceValue;
		}

//PARTICLE <-> LIGHT
		double minDistanceLight = LightRadius + ParticleRadius * 1.2;
		double maxDistanceLight = 1000000;//LightRadius + ParticleRadius * 8.0;
		
		Point_3d d{ LightPosition.x - ParticlePositions[i].x,
					LightPosition.y - ParticlePositions[i].y,
					LightPosition.z - ParticlePositions[i].z };
		double distance;
		Point_3d dir = normalizeVector(d, distance);

		double forceValue = InteractionForce(distance, minDistanceLight, maxDistanceLight, 900.0 * 1.5, 40.0 * 1.5);
		//particle forces
		forces[i].x += dir.x * forceValue;
		forces[i].y += dir.y * forceValue;
		forces[i].z += dir.z * forceValue;
		
		//LIGHT не підчиняється силам симуляції - її позицію задає курсор
	}


	double Resistance = 0.1; //опір щоб частинка поступово сповільнювалась, ближче до нуля - більш пружиннисте якщо частинки взаємодіють між собою
	for(int i = 0; i < ParticleCount; i++) {
		ParticleSpeed3D[i].x = (ParticleSpeed3D[i].x + forces[i].x * simSpeed) * (1.0 - Resistance);
		ParticleSpeed3D[i].y = (ParticleSpeed3D[i].y + forces[i].y * simSpeed) * (1.0 - Resistance);
		ParticleSpeed3D[i].z = (ParticleSpeed3D[i].z + forces[i].z * simSpeed) * (1.0 - Resistance);

		ParticlePositions[i].x += ParticleSpeed3D[i].x * simSpeed;
		ParticlePositions[i].y += ParticleSpeed3D[i].y * simSpeed;
		ParticlePositions[i].z += ParticleSpeed3D[i].z * simSpeed;
	}
}

void DrawFluid() {
	UpdateFluid(0.05/*швидкість симуляції по відношенню до fps*/);

	for(const Point_3d& Particle : ParticlePositions) {
		Sphere(Particle, ParticleRadius, LightPosition);
	}
	Sphere(LightPosition, LightRadius, LightPosition);
}
