classdef RobotData < handle
    %ROBOTDATA Data from firefighter robot at an instant in time.
    %   Created by RBE-2002 B17 Team 10.
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
        
        robotState = '';        % Robot state
        wallFollowerState = ''; % Wall follower state
    end
    properties (Access = private, Constant)
        radius = 0.14; % Approximate robot radius (m)
    end
    
    methods
        function obj = RobotData(x, y, h, sF, sB, sL, sR, ...
            robotState, wallFollowerState)
            % Constructs RobotData from given robot data.
            %   x = x-position (m)
            %   y = y-position (m)
            %   h = heading (rad)
            %   sF = front sonar distance (m)
            %   sB = back sonar distance (m)
            %   sL = left sonar distance (m)
            %   sR = right sonar distance (m)
            obj.pos = [x; y];
            obj.heading = h;
            
            % Sonar distances are invalid if zero
            obj.sFvalid = (sF ~= 0);
            obj.sBvalid = (sB ~= 0);
            obj.sLvalid = (sL ~= 0);
            obj.sRvalid = (sR ~= 0);
            
            % Absolute sonar positions
            ch = cos(h);
            sh = sin(h);
            rh = [ch sh; -sh ch];
            obj.sonarF = obj.pos + rh * [0; +sF];
            obj.sonarB = obj.pos + rh * [0; -sB];
            obj.sonarL = obj.pos + rh * [-sL; 0];
            obj.sonarR = obj.pos + rh * [+sR; 0];
            
            % Robot states
            obj.robotState = robotState;
            obj.wallFollowerState = wallFollowerState;
        end
        function [aln] = getAlignment(obj)
            % Returns axis alignment of robot (+x, -y, +y, -y, or none)
            sh = sin(obj.heading);
            ch = cos(obj.heading);
            axisMin = cosd(15);
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
        function plot(obj)
            % Plots robot data on current axes.
            
            % Plot robot position and heading
            theta = linspace(0,2*pi,12);
            rx = obj.pos(1);
            ry = obj.pos(2);
            cx = rx + obj.radius * cos(theta);
            cy = ry + obj.radius * sin(theta);
            hx = rx + [0 obj.radius * sin(obj.heading)];
            hy = ry + [0 obj.radius * cos(obj.heading)];
            plot(cx, cy, 'color', 'm', 'linewidth', 2)
            plot(hx, hy, 'color', 'm', 'linewidth', 2)

            % Plot sonar vectors from robot to walls
            plot([rx obj.sonarF(1)], [ry obj.sonarF(2)], '--kx')
            plot([rx obj.sonarB(1)], [ry obj.sonarB(2)], '--kx')
            plot([rx obj.sonarL(1)], [ry obj.sonarL(2)], '--kx')
            plot([rx obj.sonarR(1)], [ry obj.sonarR(2)], '--kx')
        end
    end
end