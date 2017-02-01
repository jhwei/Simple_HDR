clc;
close all;
clear all;

test_set='./test_set3/';
fidin=fopen([test_set 'list.txt'],'r');
count=0;
image_set=zeros(101,101,3,13);
t=[];
images=1;
height=3456;
width=5184;

while ~feof(fidin)
    tline=fgetl(fidin);
    if count==0
        a=imread([test_set tline]);
        for i=height/2-50:height/2+50
            for j=width/2-50:width/2+50
                image_set(i-(height/2-51),j-(width/2-51),:,images)=a(i,j,:);
            end
        end
        name=[test_set 'crop_' num2str(images) '.jpg'];
        imwrite(uint8(image_set(:,:,:,images)),name);
        images=images+1;
        count=1;
    else
        t(end+1,1)=1/str2num(tline);
        count=0;
    end
    
end
fclose(fidin);

light=zeros(13,1);

for num=1:3
    for images=1:13
        sum=0;
        for i=1:101
            for j=1:101
                sum=sum+image_set(i,j,num,images);
            end
        end
        light(images)=sum/101/101;
    end
    
    figure;
    i=1:13;
    plot(t(i),light(i));
    title('brightness against exposure time');
    xlabel('exposure time T');
    ylabel('brightness B');
    
   % saveas(gcf,[test_set 'plot_b_t.jpg']);
    
    figure;
    plot(log(t(i)),log(light(i)));
    title('log(B) against log(T)');
    xlabel('log(T)');
    ylabel('log(B)');
    p=polyfit(log(t),log(light),1);
    x1 = linspace(-6.5,-3);
    y1 = polyval(p,x1);
    
    hold on;
    plot(x1,y1);
    hold off;
    
    %saveas(gcf,[test_set 'plot_log_' num2str(num) '.jpg']);
    
    p
    figure;
    plot(t(i),light(i).^(1/p(1)));
    title('Actual brightness against exposure time')
    xlabel('exposure time T');
    ylabel('actual brightness B');
    
    % saveas(gcf,[test_set 'plot_ac.jpg']);
end