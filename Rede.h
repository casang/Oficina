#define DEBUG

class Rede
{
  EthernetServer *server;
  int posStrLine;
  char cmd;
  int channelCmd;
  int valueCmd;
  int stateParseURL;
  bool active;
public:
  Rede ();
  void loop ();
  bool start ();
  void parseURL (char c, bool reset = false);
};
