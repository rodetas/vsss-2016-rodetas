#include "Control.h"

Control::Control(){	
	objects.resize(7);
	movements.resize(3);
	manipulation.loadCalibration();
	program = true;

}

void Control::handle(){

	calibration.setGUICalibration(&calibration);

	while(program){

		int option = menu.menu();
	
		if(option == GAME){

		// initialize classes
			vision.initialize();
			graphic.initialize();
			strategy.initialize(manipulation.getImageSize(), manipulation.getGoal());

			// game loop
			game = true;
			while(game){

			// recognize robot's points
				vision.makeVision(); 
			// set informations to other classes
				setInformations(); 
			// show information on screen
				graphic.gameInformation();
			// apply strategies
				strategy.handleStrategies(); 

				if(!graphic.isPause()){
					movements = strategy.getMovements();
					transmission.setMovements(movements);
				} else {
					for(int i=0 ; i<3 ; i++){
						transmission.sendMovement(i, graphic.getTestMovement(), 100100);
					}
				}

				if(graphic.getMenu()){
					vision.setCameraRelease();		
					graphic.setMenu(false);
					game = false;
				} else if(graphic.getClose()){
					vision.setCameraRelease();
					game = false;
					program = false;
				}
			}

		} else if(option == CALIBRATION){
		
		// start calibration process	
			calibration.calibrate();

		} else if(option == ARDUINO){			
			arduino.loop();

		} else if(option == TEST){
			test.loop();

		} else if(option == SIMULATOR){

        	simulator.openWindow();
			while(!simulator.getEndSimulator()){
				simulator.simulate();
				strategy.setObjects(simulator.getPositions());
				strategy.handleStrategies();
				//simulator.setInformation(strategy.getInformation());
			}
			simulator.setEndSimulator(false);
		}
	}

	for(int i=0 ; i<3 ; i++) transmission.sendMovement(i, STOPPED_MOVE,0);
	transmission.closeConnection();

	exit(0);
}

void Control::setInformations(){

	for (int i = 0; i < 3; i++){
		objects[i] = vision.getRobotTeam()[i];
		objects[i+3] = vision.getOpponent()[i];
	}
    objects[graphicBall] = vision.getBall();

	strategy.setObjects(objects);
	strategy.setPowerCurve(graphic.getPowerCurve());
	strategy.setPower(graphic.getPower());
	
	graphic.setObjects(objects);
	graphic.setTargets(strategy.getTargets());
	graphic.setFps(fps.framesPerSecond());
	graphic.setConnectionStatus(transmission.getConnectionStatus());
	graphic.setInformation(strategy.getInformation());

	vision.setRotateField(graphic.getRotateField());
}