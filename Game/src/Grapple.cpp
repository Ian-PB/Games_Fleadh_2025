#include "../include/Grapple.h"
#include <random>
#include <iostream>
#include "../include/SceneCamera.h"

Grapple::Grapple()
{   
    // Idle
    states[0].amplitude = 20 + (-5 + (rand() % 18));
    states[0].frequency = 60;
    states[0].speed = 5 + (-2 + (rand() % 11));
    states[0].thickness = 3;
    // Aiming
    states[1].amplitude = 90;
    states[1].frequency = 60;
    states[1].speed = 50;
    states[1].thickness = 2;
    // Grabbing
    states[2].amplitude = 50;
    states[2].frequency = 60;
    states[2].speed = 5;
    states[2].thickness = 5;

    originalAmp = states[0].amplitude;
    originalSpeed = states[0].speed;

    //  Set particle colors
    particleSpawnpoint.addColor(YELLOW);
}

void Grapple::setStartPos(Vector2 t_startPos, Vector2& t_userPos)
{
    userPos = &t_userPos;
    startPos = t_startPos;
}

void Grapple::shoot(std::shared_ptr<Object> t_target)
{
    float distToTarget = pointToPointDist(*userPos, t_target->getPos());

    if (distToTarget < RANGE)
    {
        aiming = true;
        grappledObject = t_target;
        currentState = 1; // Aiming
    }
}

void Grapple::release(Vector2 t_releaseDir, bool t_toPlanet)
{
    if (grappledObject)
    {
        grappledObject->released(t_releaseDir, t_toPlanet);
    
        grappledObject.reset();

        currentState = 0;
    }

    active = false;
}

void Grapple::aimTimer()
{
    if (aimClock < AIM_DURATION)
    {
        aimClock++;
    }
    else
    {
        aimClock = 0;
        aiming = false;

        active = true;
        if (!grappledObject->checkGrabbed())
        {
            grappledObject->grab();
        }

        currentState = 2; // Grabbing

        // Set Particle Values
        float dirAngle = atan2f(grappledObject->getPos().y - userPos->y, grappledObject->getPos().x - userPos->x);
        Vector2 particlePos;
        particlePos.x = userPos->x + ORBIT_LENGTH * cos(dirAngle);
        particlePos.y = userPos->y + ORBIT_LENGTH * sin(dirAngle);

        SceneCamera::screenShake(SceneCamera::SMALL_SHAKE, 30);

        dirAngle = radiansToDegrees(dirAngle); // Change to degrees
        particleSpawnpoint.setValues(particlePos, 15, dirAngle + 90);
        particleSpawnpoint.spawn();
    }
}

void Grapple::updateAggression()
{
    // Settings for aggression level
    states[0].amplitude = originalAmp * (aggression / 2.0f);
    states[0].speed = originalSpeed * (aggression / 2.0f);
}

void Grapple::update()
{
    if (aiming)
    {
        aimTimer();
    }

    updateAggression();

    updateSpline();

    if (grappledObject != nullptr)
    {
        // float distToTarget = pointToPointDist(*userPos, grappledObject->getPos());
        grappledObject->held(*userPos, ORBIT_LENGTH);
    }

    particleSpawnpoint.update();
}

void Grapple::updateSpline()
{
    float angleRad = atan2f(userPos->y - startPos.y, userPos->x - startPos.x);

    float dx;
    float dy;

    // Set the first point as fixed at userPos
    points[0] = startPos;

    // Generate sine wave points to simulate a string-like movement
    for (int i = 1; i < NUM_POINTS; i++) 
    {  // Start from 1 to keep userPos fixed
        float distFromCenter;
        if (active)
        {
            distFromCenter = 0.88f - (float)i / (NUM_POINTS - 1);
        }
        else
        {
            distFromCenter = 1.35f - (float)i / (NUM_POINTS - 1);
        }


        float damping = 1.0f - expf(-distFromCenter * 0.3f);
        float waveOffset = states[currentState].amplitude * damping * sinf(states[currentState].frequency * distFromCenter * 2 * PI - (GetTime() * states[currentState].speed) + distFromCenter * 5.0f);

        if (active)
        {
            // Rotate around the player when active
            float rotatingAngle = atan2f(grappledObject->getPos().y - userPos->y, grappledObject->getPos().x - userPos->x);

            dx = cosf(rotatingAngle);  // X direction
            dy = sinf(rotatingAngle);  // Y direction

            float x = userPos->x + distFromCenter * (GRAPPLE_LENGTH + 50) * dx;  // Spread out along the angle
            float y = userPos->y + distFromCenter * (GRAPPLE_LENGTH + 50) * dy + waveOffset;
            points[i] = { x, y };
        }
        else
        {
            dx = cosf(angleRad);  // X direction
            dy = sinf(angleRad);  // Y direction

            float x = startPos.x + distFromCenter * IDLE_LENGTH * dx;  // Spread out along the angle
            float y = startPos.y + distFromCenter * IDLE_LENGTH * dy + waveOffset;
            points[i] = { x, y };
        }
    }
}

void Grapple::draw()
{
    // if (active)
    // {
    //     DrawLineV(*userPos, grappledObject->getPos(), color);
    // }

    DrawSplineCatmullRom(points, NUM_POINTS, states[currentState].thickness, color);

    particleSpawnpoint.draw();
}



Vector2 Grapple::scaleVectorLenght(Vector2 t_startPoint, Vector2 t_endPoint,  int t_distance)
{
	// Calculate the total length of the line
	float totalLength = pointToPointDist(t_startPoint, t_endPoint);

	// Calculate the ratio of the desired length to the total length of the line
	float ratio = t_distance / totalLength;

	// Calculate the coordinates of the point at the desired length along the line
	float newX = t_startPoint.x + ratio * (t_endPoint.x - t_startPoint.x);
	float newY = t_startPoint.y + ratio * (t_endPoint.y - t_startPoint.y);

	return { newX, newY };
}