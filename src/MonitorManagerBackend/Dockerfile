FROM centos:latest
RUN mkdir -p /app/MonitorManager
COPY runtime/* /app/MonitorManager/
WORKDIR /app/MonitorManager
ENV  LD_LIBRARY_PATH=/app/MonitorManager:$LD_LIBRARY_PATH
CMD ["./MonitorManager"] 
