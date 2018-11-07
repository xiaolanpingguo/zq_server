
import xml.dom.minidom
import sys

SERVER_NAME_LIST = ['master_ss_server','world_ss_server', 'login_cs_server', 
                    'game_cs_server', 'gate_server', 'log_ss_server', 'test_server']

class GenCfg(object):

    def __init__(self, xml_cfg):
        self.server_dict = {}
        xml_dom = xml.dom.minidom.parse(xml_cfg)
        xml_root = xml_dom.documentElement

        xml_root_ele = xml_root.getElementsByTagName("Object")
        for server_ele in xml_root_ele:
            server_name = server_ele.getAttribute("name")
            server_id = server_ele.getAttribute("server_id")
            self.server_dict[server_id] = server_name 

    def gen_launch_bat(self):
        for server_id in self.server_dict:
            server_name = self.server_dict[server_id]
            DEBUG_NAME = 'Debug\n';
            file_name = 'start_' + server_name + ".bat";
            fp = open(file_name, 'w')
            fp.write('cd ' + DEBUG_NAME + '\n')
            fp.write("start " + "\"" + server_name + "_d" + "-" + server_id+ "\" " + "\"" + server_name + "_d\" " +  "\"" + "ID=" +  server_id + "\" ")
            fp.close();

    def gen_launch_sh(self):
        start_all_server = []
        stop_all_server = []
        reload_all_server = []
        for server_id in self.server_dict:
            server_name = self.server_dict[server_id]
            DEBUG_NAME = 'Debug\n';

            # start
            start_file_name = 'start_' + server_name + ".sh";
            fp = open(start_file_name, 'w')
            fp.write('cd ' + DEBUG_NAME + '\n')
            fp.write('echo start ' + server_name + '...\n')
            fp.write("./" + server_name + " " + "ID=" + server_id + " " + "cmd=start -d")
            fp.close();
            start_all_server.append("./" + start_file_name);

            # stop
            stop_file_name = 'stop_' + server_name + ".sh";
            fp = open(stop_file_name, 'w')
            fp.write('cd ' + DEBUG_NAME + '\n')
            fp.write('echo stop ' + server_name + '...\n')
            fp.write("./" + server_name + " " + "ID=" + server_id + " " + "cmd=stop")
            fp.close();
            stop_all_server.append("./" + stop_file_name);

            # reload
            reload_file_name = 'reload_' + server_name + ".sh";
            fp = open(reload_file_name, 'w')
            fp.write('cd ' + DEBUG_NAME + '\n')
            fp.write('echo reload ' + server_name + '...\n')
            fp.write("./" + server_name + " " + "ID=" + server_id + " " + "cmd=reload")
            fp.close();
            reload_all_server.append("./" + reload_file_name);

        # start_all
        START_ALL_NAME = 'start_all.sh';
        fp = open(START_ALL_NAME, 'w')
        for start_name in start_all_server:
            fp.write(start_name + '\n')
        fp.close();

        # stop_all
        STOP_ALL_NAME = 'stop_all.sh';
        fp = open(STOP_ALL_NAME, 'w')
        for stop_name in stop_all_server:
            fp.write(stop_name + '\n')
        fp.close();

        # reload_all
        RELOAD_ALL_NAME = 'reload_all.sh';
        fp = open(RELOAD_ALL_NAME, 'w')
        for reload_name in start_all_server:
            fp.write(reload_name + '\n')
        fp.close();

    def gen_debug_sh(self):
        for server_id in self.server_dict:
            server_name = self.server_dict[server_id]
            debug_file_name = 'debug_' + server_name + ".sh";
            DEBUG_NAME = 'Debug';
            fp = open(debug_file_name, 'w')
            fp.write('gdb --args ' + DEBUG_NAME + '/' + server_name + " ID=" + server_id + " cmd=start")
            fp.close();


if __name__ == '__main__':
    xml_cfg = sys.argv[1]
    gen_cfg = GenCfg(xml_cfg)
    gen_cfg.gen_launch_bat()
    gen_cfg.gen_launch_sh()
    gen_cfg.gen_debug_sh()