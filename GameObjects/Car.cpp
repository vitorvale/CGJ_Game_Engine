#include "Car.h"
#define FLOAT_MAX_VALUE 99999999999999999999999999999999999.99
#define FLOAT_MIN_VALUE -99999999999999999999999999999999999.99

Car::Car(MyMesh* cubeMesh, MyMesh* torusMesh, MyMesh* rearMesh, bool isCollisionEnabled_in, float radius) {
	bodyMesh = cubeMesh;
	wheelMesh = torusMesh;

	// body material
	float amb[] = { 1.0f, 0.2f, 0.2f, 1.0f };
	float diff[] = { 1.0f, 0.2f, 0.2f, 1.0f };
	float spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	memcpy(bodyMaterial.ambient, amb, 4 * sizeof(float));
	memcpy(bodyMaterial.diffuse, diff, 4 * sizeof(float));
	memcpy(bodyMaterial.specular, spec, 4 * sizeof(float));
	memcpy(bodyMaterial.emissive, emissive, 4 * sizeof(float));
	bodyMaterial.shininess = 0.0f;
	bodyMaterial.texCount = 0;

	float body_albedo[] = { 1.0f, 0.2f, 0.2f, 1.0f };
	float body_metallic = 0.5f;
	float body_roughness = 0.15f;
	memcpy(bodyMaterial.albedo, body_albedo, 4 * sizeof(float));
	bodyMaterial.metallic = body_metallic;
	bodyMaterial.roughness = body_roughness;
	bodyMaterial.ao = 1.0f;

	// top material
	float amb2[] = { 0.5f, 0.2f, 1.0f, 1.0f };
	float diff2[] = { 0.5f, 0.2f, 1.0f, 1.0f };
	float spec2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emissive2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	memcpy(topMaterial.ambient, amb2, 4 * sizeof(float));
	memcpy(topMaterial.diffuse, diff2, 4 * sizeof(float));
	memcpy(topMaterial.specular, spec2, 4 * sizeof(float));
	memcpy(topMaterial.emissive, emissive2, 4 * sizeof(float));
	topMaterial.shininess = 0.0f;
	topMaterial.texCount = 0;

	float top_albedo[] = { 0.5f, 0.2f, 1.0f, 1.0f };
	float top_metallic = 0.5f;
	float top_roughness = 0.2f;
	memcpy(topMaterial.albedo, top_albedo, 4 * sizeof(float));
	topMaterial.metallic = top_metallic;
	topMaterial.roughness = top_roughness;
	topMaterial.ao = 1.0f;

	// wheel material
	float amb3[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float diff3[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float spec3[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emissive3[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	memcpy(wheelMaterial.ambient, amb3, 4 * sizeof(float));
	memcpy(wheelMaterial.diffuse, diff3, 4 * sizeof(float));
	memcpy(wheelMaterial.specular, spec3, 4 * sizeof(float));
	memcpy(wheelMaterial.emissive, emissive3, 4 * sizeof(float));
	wheelMaterial.shininess = 0.0f;
	wheelMaterial.texCount = 0;

	float wheel_albedo[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float wheel_metallic = 0.0f;
	float wheel_roughness = 0.7f;
	memcpy(wheelMaterial.albedo, wheel_albedo, 4 * sizeof(float));
	wheelMaterial.metallic = wheel_metallic;
	wheelMaterial.roughness = wheel_roughness;
	wheelMaterial.ao = 1.0f;

	isCollisionEnabled = isCollisionEnabled_in;

	wheel_radius = radius;
	wheel_rot_speed = speed / wheel_radius * 40.0f;

	float slPos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float slPos2[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float slDir[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	sl1 = SpotLight(slPos, slDir);
	sl2 = SpotLight(slPos2, slDir);

	graphRoot = new Root(NULL, NULL, position, 0.0f);
	graphRoot->body->mesh = bodyMesh;
	graphRoot->body->material = &bodyMaterial;
	graphRoot->top->mesh = bodyMesh;
	graphRoot->top->material = &topMaterial;
	graphRoot->topLeft->mesh = torusMesh;
	graphRoot->topLeft->material = &wheelMaterial;
	graphRoot->topRight->mesh = torusMesh;
	graphRoot->topRight->material = &wheelMaterial;
	graphRoot->botLeft->mesh = torusMesh;
	graphRoot->botLeft->material = &wheelMaterial;
	graphRoot->botRight->mesh = torusMesh;
	graphRoot->botRight->material = &wheelMaterial;

	graphRoot->rearMirror->mesh = rearMesh;
	graphRoot->rearMirror->material = &wheelMaterial;
}

//float* Car::GetPosition() { return position; }
float* Car::GetPosition() { return graphRoot->position; }
void Car::SetMeshMaterial(MyMesh* mesh, Material mat) {	mesh->mat = mat;}
//Material Car::GetBodyMaterial() { return bodyMaterial; }
Material Car::GetBodyMaterial() { return *graphRoot->body->material; }
//Material Car::GetTopMaterial() { return topMaterial; }
Material Car::GetTopMaterial() { return *graphRoot->top->material; }
//Material Car::GetWheelMaterial() { return wheelMaterial; }
Material Car::GetWheelMaterial() { return *graphRoot->topLeft->material; }
//MyMesh* Car::GetBodyMesh() { return bodyMesh; }
MyMesh* Car::GetBodyMesh() { return graphRoot->body->mesh; }
//MyMesh* Car::GetWheelMesh() { return wheelMesh; }
MyMesh* Car::GetWheelMesh() { return graphRoot->topLeft->mesh; }

MyMesh* Car::GetRearMesh() { return graphRoot->rearMirror->mesh; }

LightProperties* Car::GetSpotLight1() {
	return sl1.GetLightPtr();
}
LightProperties* Car::GetSpotLight2() {
	return sl2.GetLightPtr();
}

void Car::UpdateSpotLights()
{
	pushMatrix(MODEL);
	loadMatrix(MODEL, graphRoot->body->m_model);
	sl1.UpdateTransform(MODEL, sl1_offset);
	sl2.UpdateTransform(MODEL, sl2_offset);
	popMatrix(MODEL);

	float dir[4];
	dir[0] = direction[0];
	dir[1] = direction[1];
	dir[2] = direction[2];
	dir[3] = 0.0f;
	sl1.SetConeDirection(dir);
	sl1.GetLightPtr()->spotCosCutoff = 0.85f;
	sl2.SetConeDirection(dir);
	sl2.GetLightPtr()->spotCosCutoff = 0.85f;
}

void Car::MoveCar()
{
	float dt = TimeUtil::deltaTime;
	float dx = 0.0f;
	float dz = 0.0f;

	rotationAngle += steerChange * dt;
	float radAngle = DegToRad(rotationAngle);

	if (radAngle > PI * 2) {
		radAngle -= PI * 2;
	}
	else if (radAngle < PI * 2) {
		radAngle += PI * 2;
	}

	float sn = sin(radAngle);
	float cs = cos(radAngle);

	direction[0] = cs;
	direction[2] = sn;
	normalize(direction);

	if (throttle == 0 && brake == 0 && speed != 0) {
		float sign = speed > 0 ? -1.0f : 1.0f;
		speed += sign * 0.5f * acceleration * dt;
		if (abs(speed) < 0.1f) {
			speed = 0.0f;
		}
	}
	else {
		speed += (throttle - brake) * acceleration * dt;
		if (abs(speed) > maxSpeed) {
			speed = maxSpeed;
		}
	}
	wheel_rot_speed = speed / wheel_radius * 40.0f;

	float distance = speed * dt;

	dx = distance * cs;
	dz = distance * sn;

	float position_offset[3] = { dx, 0.0f, dz };

	std::copy(position, position + 3, old_position);
	add(position, position_offset, position);

	memcpy(graphRoot->position, this->position, sizeof(float) * 3);
	graphRoot->rotationAngle = this->rotationAngle;
}

void Car::Update() {
	float aux[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	graphRoot->Update();
	UpdateSpotLights();
	wheel_rot_angle += wheel_rot_speed * TimeUtil::deltaTime;
	graphRoot->topLeft->angle = wheel_rot_angle;
	graphRoot->topRight->angle = wheel_rot_angle;
	graphRoot->botLeft->angle = wheel_rot_angle;
	graphRoot->botRight->angle = wheel_rot_angle;

	pushMatrix(MODEL);
	loadMatrix(MODEL, graphRoot->body->m_model);

	for (int k = 0; k < 8; k++) {
		std::copy(bb_identity[k], bb_identity[k] + 4, bb[k]);
	}

	for (int k = 0; k < 8; k++) {
		multMatrixPoint(MODEL, bb[k], aux);
		std::copy(aux, aux + 4, bb[k]);
	}

	popMatrix(MODEL);
}

void Car::CalculateBoundingBox() {
	float min_x = FLOAT_MAX_VALUE, max_x = FLOAT_MIN_VALUE, min_y = FLOAT_MAX_VALUE, max_y = FLOAT_MIN_VALUE, min_z = FLOAT_MAX_VALUE
		, max_z = FLOAT_MIN_VALUE;

	for (int k = 0; k < 8; k++) {
		if (bb[k][0] < min_x) {
			min_x = bb[k][0];
		}

		if (bb[k][0] > max_x) {
			max_x = bb[k][0];
		}
		if (bb[k][1] < min_y) {
			min_y = bb[k][1];
		}
		if (bb[k][1] > max_y) {
			max_y = bb[k][1];
		}
		if (bb[k][2] < min_z) {
			min_z = bb[k][2];
		}
		if (bb[k][2] > max_z) {
			max_z = bb[k][2];
		}

	}

	bb_bounds[0] = min_x;
	bb_bounds[1] = max_x;
	bb_bounds[2] = min_y;
	bb_bounds[3] = max_y;
	bb_bounds[4] = min_z;
	bb_bounds[5] = max_z;
}

void Car::PrintBB()
{
	printf("{\n");
	for (int k = 0; k < 8; k++) {
		printf("{");
		for (int i = 0; i < 3; i++) {
			printf("[%f] ", bb[k][i]);
		}
		printf("}\n");
	}
	printf("}\n");
}

bool Car::DetectCollisionWithObject(GameObject3D* other)
{

	if (other->GetBBType() == GameObject3D::BB_TYPE::Spherical && other->GetType() == GameObject3D::TYPE::Orange) {
		// get box closest point to sphere center by clamping
		float x = std::max(bb_bounds[0], std::min(other->GetPosition()[0], bb_bounds[1]));
		float y = std:: max(bb_bounds[2], std::min(other->GetPosition()[1], bb_bounds[3]));
		float z = std::max(bb_bounds[4], std::min(other->GetPosition()[2], bb_bounds[5]));
		float aux[3];
		float distance[3] = { 0.0f, 0.0f, 0.0f };
		
		aux[0] = x;
		aux[1] = y;
		aux[2] = z;

		subtract(aux, other->GetPosition(), distance);

		Orange *o = dynamic_cast<Orange*>(other);
	

		return length(distance) < o->getRadius();
	}
	else if (other->GetBBType() == GameObject3D::BB_TYPE::Spherical && other->GetType() == GameObject3D::TYPE::Cheerio) {
		// get box closest point to sphere center by clamping
		float x = std::max(bb_bounds[0], std::min(other->GetPosition()[0], bb_bounds[1]));
		float y = std::max(bb_bounds[2], std::min(other->GetPosition()[1], bb_bounds[3]));
		float z = std::max(bb_bounds[4], std::min(other->GetPosition()[2], bb_bounds[5]));
		float aux[3];
		float distance[3] = { 0.0f, 0.0f, 0.0f };

		aux[0] = x;
		aux[1] = y;
		aux[2] = z;

		subtract(aux, other->GetPosition(), distance);

		Cheerios *c = dynamic_cast<Cheerios*>(other);

		return length(distance) < c->getRadius();
	}
	else{
		Table* t = dynamic_cast<Table*>(other);
		return (bb_bounds[0] <= t->GetBBBounds()[1] && bb_bounds[1] >= t->GetBBBounds()[0]) &&
			(bb_bounds[2] <= t->GetBBBounds()[3] && bb_bounds[3] >= t->GetBBBounds()[2]) &&
			(bb_bounds[4] <= t->GetBBBounds()[5] && bb_bounds[5] >= t->GetBBBounds()[4]);
	}

	return false;
}

void Car::ResolveCollisionWithObject(GameObject3D* other)
{
	if (other->GetType() == GameObject3D::TYPE::Table) {
		Table* t = dynamic_cast<Table*>(other);
		t->SetSpeed(1.5f);
		if(speed>=0)
			t->SetDirection(direction);
		else{
			float opositeDirection[3] = { 0.0f, 0.0f, 0.0f };
			constProduct(-1, direction, opositeDirection);
			t->SetDirection(opositeDirection);
		}
		speed = 0;
		wheel_rot_speed = speed / wheel_radius * 40.0f;
	}
	if (other->GetType() == GameObject3D::TYPE::Cheerio) {
		Cheerios* c = dynamic_cast<Cheerios*>(other);
		c->SetSpeed(1.5f);
		if (speed >= 0)
			c->SetDirection(direction);
		else {
			float opositeDirection[3] = { 0.0f, 0.0f, 0.0f };
			constProduct(-1, direction, opositeDirection);
			c->SetDirection(opositeDirection);
		}
		speed = 0;
		wheel_rot_speed = speed / wheel_radius * 40.0f;
	}
	if (other->GetType() == GameObject3D::TYPE::Orange) {
		float new_pos[3] = { 5.0f, 0.3f, 0.0f };///!!!!!!!!!!!!!!!!!!!!!!!
		std::copy(new_pos, new_pos + 3, position);
	}
}