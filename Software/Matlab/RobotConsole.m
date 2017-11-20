%ROBOTCONSOLE Script for RBE-2002 Final Project Robot
%   Created by RBE-2002 B17 Team 10.
%   
%   This script handles runs robot user-interface, Bluetooth communication,
%   and displays script and robot status via the Matlab command line.

%% Clear Workspace
close all
clear all
clc

%% Constants
BLUETOOTH_NAME = 'Arduino';
BLUETOOTH_CHANNEL = 1;
TELEOP_VOLTAGE = 4;
MAX_LOOPS = 10000;
LOG_NAME = 'RobotLog.mat';

%% User Interface Initialization
ui = RobotUI();
robot = RobotComms(BLUETOOTH_NAME, BLUETOOTH_CHANNEL);
map = MapBuilder();

%% Connect to Robot or Replay Log
replay = 0;
while 1
    displayTitle('Awaiting input');

    if ui.connectButton()
        disp('Connecting to robot...')
        [s, msg] = robot.connect();
        disp(msg)
        pause(1)
        if s == 1
            break
        else
            robot.disconnect();
            return
        end
    elseif ui.replayButton()
        disp('Replaying last robot run...')
        replay = 1;
        pause(1)
        break
    else
        disp('Press ''Connect'' to connect to robot.')
        disp('Press ''Replay'' to simulate last run.')
    end

    ui.update();
end

%% Begin Teleop or Disconnect
if ~replay
    while 1
        displayTitle('Awaiting input');

        if ui.beginButton()
            [s, error] = robot.start();
            if s == 0
                disp(error)
                robot.disconnect();
                return
            end
            break
        elseif ui.disconnectButton()
            disp('Disconnected.')
            robot.disconnect();
            return
        else 
            disp('Press ''Begin'' to begin autonomous.')
            disp('Press ''Disconnect'' to terminate program.')
        end

        ui.update();
    end
end

%% Robot Loop (Teleop or Replay)
loopCount = 1;
if ~replay  
    robotLog = RobotData.empty(0, MAX_LOOPS);
else
    load(LOG_NAME);
    MAX_LOOPS = length(robotLog);
end
targetHeading = 0;

while 1
    if ~replay
        % Teleoperated Loop
        displayTitle('Autonomous Mode');
        disp(['Loop: ' int2str(loopCount)])
        
        % Get odometry data and update map
        [rd, s, error] = robot.getData();
        if s == 0
            disp(error)
            robot.disconnect();
            break
        end
        robotLog(loopCount) = rd;

        % Check disconnect button on UI
        if ui.disconnectButton()
            disp('Disconnect by user.')
            robot.disconnect();
            break
        end
    else
        % Replay Loop
        displayTitle('Replay Mode');
        disp(['Loop: ' int2str(loopCount)])
        
        % Get recorded robot data
        rd = robotLog(loopCount);
        
        % Check disconnect button on UI
        if ui.disconnectButton()
            disp('Simulation cancelled.')
            break
        end
    end

    % Update map if robot is moving
    if loopCount > 1
        if isequal(rd.pos, robotLog(loopCount-1).pos)
            disp('Robot Stationary!')
        else
            slip = map.update(rd);
            if norm(slip) ~= 0
                disp('Wheel slip detected!')
                if replay
                    for i = loopCount + 1 : MAX_LOOPS
                        robotLog(i).removeSlip(slip);
                    end
                else
                    % Send slip to robot?
                    % robot.sendSlippage(slip)
                end
            end
        end
    else
        map.update(rd);
    end
    
    % Status Display
    disp(['Position: (' ...
        num2str(rd.pos(1), '%.2f') ', ' ...
        num2str(rd.pos(2), '%.2f'), ')'])
    disp(['Heading: ' num2str(rad2deg(rd.heading), '%.0f')])
    disp(['Alignment: ' rd.getAlignment()])
    disp(['State: ' rd.state])

    % Update UI and plots
    cla
    hold on
    rd.plot();
    map.plot();
    ui.update();
    hold off
    
    % Increment loop count or exit if at max
    if loopCount == MAX_LOOPS
        disp('Max communication loops reached!')
        if ~replay
            robot.disconnect();
        end
        break
    else
        loopCount = loopCount + 1;
    end
end

%% Post Robot Loop

% Save robot log for replays
if ~replay
    save(LOG_NAME, 'robotLog');
    disp(['Robot log saved in ''' LOG_NAME ''''])
end

%% Helper Functions

function displayTitle(subtitle)
    % Clears command line and displays program title and subtitle
    clc
    disp('Robot Console')
    disp([subtitle newline])
end