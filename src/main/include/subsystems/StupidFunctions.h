#pragma once

#include <frc2/command/SubsystemBase.h>
#include <frc2/command/RunCommand.h>
#include <frc/XboxController.h>
#include <frc/GenericHID.h>
#include <frc/smartdashboard/SmartDashboard.h>

#include "Constants.h"

class StupidFunctions : public frc2::SubsystemBase {
    public:
        StupidFunctions();

        void Periodic() override;

        double randomStickDrift(int axis);
        void constructiveCriticism(frc::XboxController& driverController);
        void constructiveCritismKill();

        bool brickRobot = false;

        //frc2::Command* cmd = new frc2::RunCommand([this]{constructiveCriticismKill();}, {this});

    private:
        int drunkModulusCounter = 0;
        double randomAdderX;
        double randomAdderY;
        double randomAdderRot;
        
        int criticismCycles = StupidFunctionsConstants::kMaxCriticismSeconds*50; // Converts seconds to cycles (1 cycle - 20ms)
        double criticismRand = 0;
        int criticismCounter = 0;
        bool criticismStalled = false;


        bool stupidModeActive = true;
    
};