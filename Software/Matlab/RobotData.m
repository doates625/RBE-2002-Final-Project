classdef RobotData < handle
    %ROBOTDATA Data from firefighter robot at an instant in time.
    %   Created by Dan Oates (RBE-2002 B17 Team 10).
    %   
    %   This class is used in array form to log the data from the robot
    %   over an entire run for replay and post-processing purposes.
    %
    %   All units are standard SI (meters, seconds, radians, etc), and
    %   all vectors are in [x; y] format.
    
    properties
        pos = [0; 0];   % Robot position from start
        heading = 0;    % Robot heading (0 - 2pi)
        
        sFvalid = 0;    % Sonar front valid
        sBvalid = 0;    % Sonar back valid
        sLvalid = 0;    % Sonar left valid
        sRvalid = 0;    % Sonar right valid

        sonarF = [0; 0];    % Sonar position front
        sonarB = [0; 0];    % Sonar position back
        sonarL = [0; 0];    % Sonar position left
        sonarR = [0; 0];    % Sonar position right
        
        flamePos = [0; 0; 0];   % Flame position ([x; y; z])
        robotState = '';        % Robot state (string)
        wallFollowerState = ''; % Wall follower state (string)
        flameStatus = '';       % Flame status (string)
    end
    properties (Access = private, Constant)
        RADIUS = 0.14;      % Approximate robot radius (m)
        SONAR_MAX = 0.75;   % Max valid sonar distance (m)
    end
    
    methods
        function obj = RobotData(x, y, h, sF, sB, sL, sR, flamePos, ...
            robotState, wallFollowerState, flameStatus)
            % Constructs RobotData from given robot data.
            % Inputs:
            %   x = x-position (m)
            %   y = y-position (m)
            %   h = heading (rad)
            %   sF = front sonar distance (m)
            %   sB = back sonar distance (m)
            %   sL = left sonar distance (m)
            %   sR = right sonar distance (m)
            %   
            
            obj.pos = [x; y];
            obj.heading = h;
            obj.flamePos = flamePos;
            
            % Sonar distances are invalid if zero or less than SONAR_MAX
            % SONAR_MAX cutoff prevents floor-bounce data from being used
            obj.sFvalid = (sF ~= 0) && (sF <= obj.SONAR_MAX);
            obj.sBvalid = (sB ~= 0) && (sB <= obj.SONAR_MAX);
            obj.sLvalid = (sL ~= 0) && (sL <= obj.SONAR_MAX);
            obj.sRvalid = (sR ~= 0) && (sR <= obj.SONAR_MAX);
            
            % Absolute sonar positions
            ch = cos(h);
            sh = sin(h);
            rh = [ch sh; -sh ch];
            obj.sonarF = obj.pos + rh * [0; +sF];
            obj.sonarB = obj.pos + rh * [0; -sB];
            obj.sonarL = obj.pos + rh * [-sL; 0];
            obj.sonarR = obj.pos + rh * [+sR; 0];
            
            % Robot state information
            obj.robotState = robotState;
            obj.wallFollowerState = wallFollowerState;
            obj.flameStatus = flameStatus;
        end
        function [aln] = getAlignment(obj)
            % Returns axis alignment of robot (+x, -y, +y, -y, or none)
            sh = sin(obj.heading);
            ch = cos(obj.heading);
            axisMin = cosd(10);
            if sh > axisMin
                aln = '+x';
            elseif sh < -axisMin
                aln = '-x';
            elseif ch > axisMin
                aln = '+y';
            elseif ch < -axisMin
                aln = '-y';
            else
                aln = 'none';
            end
        end
        function [wf] = isWallFollowing(obj)
            % Returns 1 if robot is currently wall-following (else 0).
            wf = ~strcmp(obj.wallFollowerState, 'Stopped');
        end
        function plot(obj)
            % Plots robot data on current axes.
            
            % Plot robot position and heading
            theta = linspace(0,2*pi,12);
            rx = obj.pos(1);
            ry = obj.pos(2);
            cx = rx + obj.RADIUS * cos(theta);
            cy = ry + obj.RADIUS * sin(theta);
            hx = rx + [0 obj.RADIUS * sin(obj.heading)];
            hy = ry + [0 obj.RADIUS * cos(obj.heading)];
            plot(cx, cy, 'color', 'm', 'linewidth', 2)
            plot(hx, hy, 'color', 'm', 'linewidth', 2)

            % Plot sonar vectors from robot to walls if wall-following
            if obj.isWallFollowing()
                plot([rx obj.sonarF(1)], [ry obj.sonarF(2)], '--kx')
                plot([rx obj.sonarB(1)], [ry obj.sonarB(2)], '--kx')
                plot([rx obj.sonarL(1)], [ry obj.sonarL(2)], '--kx')
                plot([rx obj.sonarR(1)], [ry obj.sonarR(2)], '--kx')
            end
        end
    end
end