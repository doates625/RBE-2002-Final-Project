%ROBOTCONSOLE Function for RBE-2002 Final Project Robot
%   Created by RBE-2002 B17 Team 10.
%   
%   This function handles the robot user-interface, Bluetooth,
%   and SLAM for field mapping and position correction.
%
%   See also: ROBOTUI, ROBOTCOMMS, MAPBUILDER

%% Initialization
% Clear workspace
close all
clear all
clc

% 
ui = RobotUI();
robot = RobotComms('Arduino', 1);
map = MapBuilder();
logName = 'RobotLog.mat';

%% First User Input
% Options:
% - Connect to robot
% - Replay last robot run
% - Terminate program
replay = 0;
while 1
    displayTitle('Awaiting Input');

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
    elseif ui.disconnectButton()
        disp('Program terminated.')
        return
    else
        disp('Press ''Connect'' to connect to robot.')
        disp('Press ''Replay'' to simulate last run.')
        disp('Press ''Disconnect'' to terminate program.')
    end

    ui.update();
end

%% Second User Input
% Runs only if robot is connected to.
% Options:
% - Begin robot operations
% - Disconnect from robot
if ~replay
    while 1
        displayTitle('Awaiting Input');

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

%% Robot Loop (Autonomous or Replay
% Initialization
loop = 1;
if ~replay  
    maxLoops = 1000;
    robotLog = RobotData.empty(0, maxLoops);
else
    logFile = load(logName, 'robotLog');
    robotLog = logFile.robotLog;
    clear('logFile')
    maxLoops = length(robotLog);
end

% Loop
while 1
    if ~replay
        % Teleoperated Loop
        displayTitle('Autonomous Mode');
        disp(['Loop: ' int2str(loop) '/' int2str(maxLoops)])
        
        % Get odometry data and update map
        [rd, s, error] = robot.getData();
        if s == 0
            disp(error)
            robot.disconnect();
            break
        end
        robotLog(loop) = rd;

        % Check disconnect button on UI
        if ui.disconnectButton()
            disp('Disconnect by user.')
            robot.disconnect();
            break
        end
    else
        % Replay Loop
        displayTitle('Replay Mode');
        disp(['Loop: ' int2str(loop) '/' int2str(maxLoops)])
        
        % Get recorded robot data
        rd = robotLog(loop);
        
        % Check disconnect button on UI
        if ui.disconnectButton()
            disp('Simulation cancelled.')
            break
        end
    end

    % Update map if robot is moving
    if loop > 1
        if isequal(rd.pos, robotLog(loop-1).pos)
            disp('Robot Stationary!')
        else
            slip = map.update(rd);
            if norm(slip) ~= 0
                disp('Wheel slip detected!')
                if replay
                    for i = loop + 1 : maxLoops
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
    if loop == maxLoops
        disp('Max communication loops reached!')
        if ~replay
            robot.disconnect();
        end
        break
    else
        loop = loop + 1;
    end
end

%% Post Robot Loop

% Save robot log for replays
if ~replay
    save(logName, 'robotLog');
    disp(['Robot log saved in ''' logName ''''])
end

%% Helper Functions

function displayTitle(subtitle)
    % Clears command line and displays program title and subtitle
    clc
    disp('-------------------------------------------------')
    disp('ROBOT CONSOLE')
    disp(subtitle)
    disp('-------------------------------------------------')
end