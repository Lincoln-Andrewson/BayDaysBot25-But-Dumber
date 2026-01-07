#include "subsystems/StupidFunctions.h"
#include <frc/controller/PIDController.h>


StupidFunctions::StupidFunctions(){
        
}

void StupidFunctions::Periodic(){

}

double StupidFunctions::randomStickDrift(int axis) {
    //Gradually shifts randomAdder value, and if it gets too close to 0, reset it to something random
    // The frequency may need to be changed to what is best
    if(stupidModeActive == true){
        if(drunkModulusCounter % 200 == 0){
                if(axis == 0){
                if((randomAdderX <= 0.02) && (randomAdderX >= -0.02)){
                    randomAdderX = (((double)rand() / RAND_MAX) * 2) - 1;
                }
                if(randomAdderX >= 0){
                    randomAdderX = randomAdderX - 0.01;
                } else if(randomAdderX <= 0){
                    randomAdderX = randomAdderX + 0.01;
                }
                frc::SmartDashboard::PutNumber("rax", randomAdderX);
                return randomAdderX;
            }
            else if(axis == 1){
                if((randomAdderY <= 0.02) && (randomAdderY >= -0.02)){
                    randomAdderY = (((double)rand() / RAND_MAX) * 2) - 1;
                }
                else{
                if(randomAdderY >= 0){
                    randomAdderY = randomAdderY - 0.01;
                } else if(randomAdderY <= 0){
                    randomAdderY = randomAdderY + 0.01;
                }
                }
                frc::SmartDashboard::PutNumber("ray", randomAdderY);
                return randomAdderY;
            }
            else if (axis == 2){
                if((randomAdderRot <= 0.02) && (randomAdderRot >= -0.02)){
                    randomAdderRot = (((double)rand() / RAND_MAX) * 2) - 1;
                }
                else{
                if(randomAdderRot >= 0){
                    randomAdderRot = randomAdderRot - 0.01;
                } else if(randomAdderRot <= 0){
                    randomAdderRot = randomAdderRot + 0.01;
                }
                }
                frc::SmartDashboard::PutNumber("rar", randomAdderRot);
                return randomAdderRot;
            }
             
        } else {
            return 0.0;
        }
    } else if (axis == 0){
        frc::SmartDashboard::PutNumber("rax", randomAdderX);
        return randomAdderX;
    } else if (axis == 1){
        frc::SmartDashboard::PutNumber("ray", randomAdderY);
        return randomAdderY;
    }
    }

void constructiveCriticismKill(){
    frc::SmartDashboard::PutBoolean("HAHAHAHA DEAD", "yup");
    if(int zero = 0; zero==0){
        5/zero;
    }
    bool criticismStalled = false;
}
void StupidFunctions::constructiveCriticism(frc::XboxController& driverController){  
    if(stupidModeActive == true){
        if(!criticismStalled){
            if(criticismCounter >= criticismCycles){  // If critismCounter is over the max time, reset it
                criticismCounter = 0;
            } 
            if(criticismCounter == 0){ // Set the random on the first pass
                criticismRand = ((double)rand() / RAND_MAX)*criticismCycles; 
            }

            if(frc::ApplyDeadband(criticismRand-criticismCounter, StupidFunctionsConstants::kRumbleLength) == 0){ // If the counter is in between a range (rumble length) of the random point, rumble controller 
                driverController.SetRumble(driverController.kBothRumble, 1);
                frc::SmartDashboard::PutNumber("Rumble", 1);
                //criticismStalled = true; //If this is enabled it will crash the code when the controller rumbles

            } else{  // Otherwise stop rumbling controller
                driverController.SetRumble(driverController.kBothRumble, 0);
                frc::SmartDashboard::PutNumber("Rumble", 0);
            }

            frc::SmartDashboard::PutNumber("Random", criticismRand);
            frc::SmartDashboard::PutNumber("Counter", criticismCounter);
            frc::SmartDashboard::PutBoolean("Stalled", criticismStalled);
            criticismCounter++;

        } else{ // I don't even know
            driverController.SetRumble(driverController.kBothRumble, 0);
            frc::SmartDashboard::PutString("YOU SCREWED UP XD", "Fibler Text");
            
        }
    }
};