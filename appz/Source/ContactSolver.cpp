#include "ContactSolver.h"
/****************************************************************************/
/*!
\file ContactSolver.cpp
\author Muhammad Shafik Bin Mazlinan
\par email: cyboryxmen@yahoo.com
\brief
A class used to solve and handle contacts
*/
/****************************************************************************/

ContactSolver::ContactSolver()
{
}

ContactSolver::~ContactSolver()
{
}

void ContactSolver::SetVelocitiesOfCollidedBodies()
{
	//for(l
	//{
		//(*collidedBody)->ReactToCollisionWith
	//}
}

bool ContactSolver::CheckThisCollision(CollisionBody* body1, CollisionBody* body2, const double deltaTime)
{
	//Contact contact;
	//if(body1->GetDiameter() > body2->GetDiameter())
	//{
	//	contact = body1->DoCollisionWith(body2, deltaTime);
	//}
	//else
	//{
	//	contact = body2->DoCollisionWith(body1, deltaTime);
	//}
	//if(!(contact == Contact()))
	//{
	//	list.InsertNode(contact);
	//}
	return false;
}

bool ContactSolver::CheckThisCollision(Voxel* voxel1, Voxel* voxel2, const double deltaTime)
{
	//const double numOfSteps = 1000;
	//const double timestep = 0.001;
	//double timeOfImpact = 0.0;
	//bool update = true;
	//voxel1->ApplyCurrentMatrix();
	//voxel2->ApplyCurrentMatrix();
	//while(!voxel1->At(timeOfImpact).IsCollidingWith(voxel2->At(timeOfImpact)) && timeOfImpact < deltaTime)
	//{
	//	timeOfImpact += timestep;
	//}
	////if collision is found
	//if(timeOfImpact < deltaTime)
	//{
	//	list.InsertNode(Contact(voxel1, voxel2, timeOfImpact));
	//	return true;
	//}
	return false;
}

void ContactSolver::ResolveAllCollisionsAccordingTo(const double deltaTime)
{
	//for(LinkList<voxel>;
}