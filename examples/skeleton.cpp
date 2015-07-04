#include "delegator/delegator.h"

class Process : public Coordinator_Worker_Process {};

int main ( int argc, char * argv [] ) {
  delegator::Start ();
  delegator::Run < Process > (argc, argv);
  delegator::Stop ();
}
