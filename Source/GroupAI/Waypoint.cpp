// Fill out your copyright notice in the Description page of Project Settings.

#include "Waypoint.h"


AWaypoint::AWaypoint()
{
	beingVisited = false;
}

int AWaypoint::GetWaypointPath()
{
	return WaypointNumber;
}
