void
BackoffTimer::start(int cw, int idle, double difs)
{
        Scheduler &s = Scheduler::instance();

        assert(busy_ == 0);

        busy_ = 1;
        paused_ = 0;
        stime = s.clock();

        if(mac->misbhType_ == 1){
                //If Type 1 Misbehavior, then Alpha Misbehavior.
                cw = (int)(mac->misbhValue_ * cw);
                if(cw == 0) {
                        cw = 1;
                }
                rtime = (Random::random() % cw) * mac->phymib_.getSlotTime();
        } else if(mac->misbhType_ == 2){
                //If Type 2 Misbehavior, then deterministic Misbehavior. No random selection of backoff timer.
                rtime = (mac->misbhValue_) * (mac->phymib_.getSlotTime());
        } else if(mac->misbhType_ == 3){
                //If Type 3 Misbehavior, then Beta Misbehavior.
                cw = max(mac->phymib_.getCWMin(),min((u_int32_t)(mac->misbhValue_ * cw),mac->phymib_.getCWMax()));
                if(cw == 0) {
                        cw = 1;
                }
                rtime = (Random::random() % cw) * mac->phymib_.getSlotTime();
        } else if(mac->misbhType_ == 4){
                //If Type 4 Misbehavior, the adjustment is done in TCL Script. So do the regular work.
                rtime = (Random::random() % cw) * mac->phymib_.getSlotTime();
        } else if(mac->misbhType_ == 5){
                //If Type 5 Misbehavior (fixed CW), then fix the CW to the user provided CW value.
                cw = (int)mac->misbhValue_;
                if(cw == 0) {
                        cw = 1;
                }
                rtime = (Random::random() % cw) * mac->phymib_.getSlotTime();
        } else {
                //For no misbehavior, do the regular rtime calculation.
                //printf("\ncw %d misbhType:%d misbhValue:%f phymisbhType:%d phymisbhValue:%d",cw, mac->misbhType_,mac->misbhValue_,mac->phymib_.getMisbhType(),mac->phymib_.getMisbhValue());
                rtime = (Random::random() % cw) * mac->phymib_.getSlotTime();
        }

#ifdef USE_SLOT_TIME
        ROUND_TIME();
#endif
        difs_wait = difs;

        if(idle == 0)
                paused_ = 1;
        else {
                assert(rtime + difs_wait >= 0.0);
                s.schedule(this, &intr, rtime + difs_wait);
        }
}
