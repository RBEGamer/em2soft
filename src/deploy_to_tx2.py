import argparse
import pysftp
import sys
import os
import uuid


print(sys.argv)


uid= str(uuid.uuid4())
try:
    srv = pysftp.Connection(host=sys.argv[1], username=sys.argv[2],password=sys.argv[3])
    os.system("zip -r /tmp/"+uid+".zip ~/Desktop/em2soft/src/ros_workspace ~/Desktop/em2soft/src/ros_workspace  ~/Desktop/em2soft/tablet_ui_server");
    sftp.put("/tmp/"+uid+".zip", "/tmp/"+uid+".zip")
    srv.execute("unzip /tmp/"+uid+".zip")
    srv.execute("bash /tmp/"+uid+"/ros_workspace/en_pkg_env.bash")
    srv.execute("bash /tmp/"+uid+"/ros_workspace/build_ros_nodes.bash")
    srv.execute("bash /tmp/"+uid+"/ros_workspace/src/run_ros_launch.sh")
except:
    pass

srv.close()

