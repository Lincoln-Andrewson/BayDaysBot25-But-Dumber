// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/controller/PIDController.h>
#include <frc/geometry/Translation2d.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/trajectory/Trajectory.h>
#include <frc/trajectory/TrajectoryGenerator.h>
#include <frc2/command/InstantCommand.h>
#include <frc2/command/SequentialCommandGroup.h>
#include <frc2/command/SwerveControllerCommand.h>
#include <frc2/command/button/JoystickButton.h>
#include <units/angle.h>
#include <units/velocity.h>

#include <algorithm>
#include <frc/smartdashboard/SmartDashboard.h>

#include <utility>

#include "Constants.h"
#include "subsystems/DriveSubsystem.h"

using namespace DriveConstants;


RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  // The left stick controls translation of the robot.
  // Turning is controlled by the X axis of the right stick.
  m_drive.SetDefaultCommand(frc2::RunCommand(
      [this] {
        m_drive.Drive(
            -units::meters_per_second_t{std::clamp(frc::ApplyDeadband(
                m_driverController.GetLeftY(), OIConstants::kDriveDeadband)+(randomStickDrift(1) * 0.4), -1.0, 1.0)},
                //Applies deadband to controller to rmove stick drift, then adds a random double number 0-1 and clamp it so it doesn't go over 1
            -units::meters_per_second_t{std::clamp(frc::ApplyDeadband(
                m_driverController.GetLeftX(), OIConstants::kDriveDeadband)+(randomStickDrift(0) * 0.4), -1.0, 1.0)},
            -units::radians_per_second_t{frc::ApplyDeadband(
                std::clamp(m_driverController.GetRightX()+(randomStickDrift(2)*0.3), -1.0, 1.0), OIConstants::kDriveDeadband)},
            true);
      },
      {&m_drive}));
    
    m_shooter.SetDefaultCommand(frc2::RunCommand([this] {
        ShooterControl();
    }, {&m_shooter}));
}

void RobotContainer::ShooterControl(){
    m_shooter.SetShooterSpeed(frc::ApplyDeadband(m_driverController.GetLeftTriggerAxis(), OIConstants::kShooterDeadband));
}

void RobotContainer::ConfigureButtonBindings() {
    frc2::JoystickButton(&m_driverController,
                       frc::XboxController::Button::kLeftBumper)
      .WhileTrue(new frc2::RunCommand([this] { m_drive.SetX(); }, {&m_drive}));

    frc2::JoystickButton(&m_driverController,
    frc::XboxController::Button::kRightBumper).WhileTrue(new frc2::RunCommand([this] {
                        m_loader.RunLoader();
    },
    {&m_loader})).OnFalse(new frc2::RunCommand([this] {
                        m_loader.StopLoader();
    },
    {&m_loader}));

    frc2::JoystickButton(&m_driverController, frc::XboxController::Button::kY).OnTrue(
    new frc2::InstantCommand([this] {
        drunkModeActive = !drunkModeActive;
        frc::SmartDashboard::PutBoolean("IS IT DRUNK REAL GOOD?!", drunkModeActive);
    })
    );
                    

    
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // Set up config for trajectory
  frc::TrajectoryConfig config(AutoConstants::kMaxSpeed,
                               AutoConstants::kMaxAcceleration);
  // Add kinematics to ensure max speed is actually obeyed
  config.SetKinematics(m_drive.kDriveKinematics);

  // An example trajectory to follow.  All units in meters.
  auto exampleTrajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      // Start at the origin facing the +X direction
      frc::Pose2d{0_m, 0_m, 0_deg},
      // Pass through these two interior waypoints, making an 's' curve path
      {frc::Translation2d{1_m, 1_m}, frc::Translation2d{2_m, -1_m}},
      // End 3 meters straight ahead of where we started, facing forward
      frc::Pose2d{3_m, 0_m, 0_deg},
      // Pass the config
      config);

  frc::ProfiledPIDController<units::radians> thetaController{
      AutoConstants::kPThetaController, 0, 0,
      AutoConstants::kThetaControllerConstraints};

  thetaController.EnableContinuousInput(units::radian_t{-std::numbers::pi},
                                        units::radian_t{std::numbers::pi});

  frc2::SwerveControllerCommand<4> swerveControllerCommand(
      exampleTrajectory, [this]() { return m_drive.GetPose(); },

      m_drive.kDriveKinematics,

      frc::PIDController{AutoConstants::kPXController, 0, 0},
      frc::PIDController{AutoConstants::kPYController, 0, 0}, thetaController,

      [this](auto moduleStates) { m_drive.SetModuleStates(moduleStates); },

      {&m_drive});

  // Reset odometry to the starting pose of the trajectory.
  m_drive.ResetOdometry(exampleTrajectory.InitialPose());

  // no auto
  return new frc2::SequentialCommandGroup(
      std::move(swerveControllerCommand),
      frc2::InstantCommand(
          [this]() { m_drive.Drive(0_mps, 0_mps, 0_rad_per_s, false); }, {}));
}

double RobotContainer::randomStickDrift(int axis) {
    //Gradually shifts randomAdder value, and if it gets too close to 0, reset it to something random
    // The frequency may need to be changed to what is best
    if(modulusCounter % 200 == 0){
        if(drunkModeActive == true){
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
        modulusCounter++;
    } else if (axis == 0){
        frc::SmartDashboard::PutNumber("rax", randomAdderX);
        return randomAdderX;
    } else if (axis == 1){
        frc::SmartDashboard::PutNumber("ray", randomAdderY);
        return randomAdderY;
    }

}
