//####//
	parent->bind("BeaconInterval_", &BeaconInterval);
	//Setting value in from TCL
	parent->bind("MisbhType_", &MisbhType);
	parent->bind("MisbhValue_", &MisbhValue);
	parent->bind("MisbhTime_", &MisbhTime);
	parent->bind("MisbhReact_", &MisbhReact); 
	//####//
	infra_mode_ = 0;	
	cw_ = phymib_.getCWMin();
	//Initializing variables
	misbhType_ = phymib_.getMisbhType();
	misbhValue_ = phymib_.getMisbhValue();
	misbhTime_ = phymib_.getMisbhTime();
	misbhReact_ = phymib_.getMisbhReact();
	misbhSmaPacketCount_ = 0;
	misbhSmaPrevTime_ = 0;
	misbhSmaIpttime_ = 0;
	misbhSmaVarSum_ = 0;
	misbhSmaVar_ = 0;
	misbhSmaSD_ = 0;
	misbhRxPacketCount_ = 0;
	misbhReactCount_ = 0;
	misbhReactionFactor_ = 0;
	misbhMaxIptRatio_ = 0.0;
	misbhMinIptRatio_ = 100.0;
//####//
	cache_node_count_ = 0;
	client_list = NULL;
	ap_list = NULL;
	queue_head = NULL;
	neighborll_head = NULL;
//####//
void
Mac802_11::recv_timer()
{
	u_int32_t src; 
	hdr_cmn *ch = HDR_CMN(pktRx_);
	hdr_mac802_11 *mh = HDR_MAC802_11(pktRx_);
	u_int32_t dst = ETHER_ADDR(mh->dh_ra);
	u_int32_t ap_dst = ETHER_ADDR(mh->dh_3a);
	u_int8_t  type = mh->dh_fc.fc_type;
	u_int8_t  subtype = mh->dh_fc.fc_subtype;
	double misbehavior=0;
.
.
.
	if (netif_->node()->energy_model() && 
	    netif_->node()->energy_model()->adaptivefidelity()) {
		src = ETHER_ADDR(mh->dh_ta);
		netif_->node()->energy_model()->add_neighbor(src);
	}
	/* This code is added only to control the duration of misbehavior. 
	   If this feature is not needed then this code is not necessary. */
	if((phymib_.getMisbhType())) {
		if((Scheduler::instance().clock() > 0) && (Scheduler::instance().clock() < misbhTime_)) {
			misbhType_ = phymib_.getMisbhType();
			misbhValue_ = phymib_.getMisbhValue();
		} else {
			misbhType_ = 0;
			misbhValue_ = 0;
		}
	}
	/* Following used by genuine nodes */
	if((!(phymib_.getMisbhType())) && (subtype == MAC_Subtype_RTS)) {
		src = ETHER_ADDR(mh->dh_ta);
		if((src) && (Address::instance().get_nodeaddr(((MobileNode*)(netif_->node()))->nodeid()))) {
			/* packets from node 0 are not used for calculation */
			//if(((Address::instance().get_nodeaddr(((MobileNode*)(netif_->node()))->nodeid())) != 2) && (src == 2)){
			misbhRxPacketCount_++;
			//}
			calc_neighbor_sma_ipttime(src);
			//print_neighborllist();
			if(((Address::instance().get_nodeaddr(((MobileNode*)(netif_->node()))->nodeid())) != 2) && (src == 2)){
				//	printf("\nNodeID: %d RxRTSCount: %f",(Address::instance().get_nodeaddr(((MobileNode*)(netif_->node()))->nodeid())),misbhRxPacketCount_); 
			}
			//if((fmod(misbhRxPacketCount_,100)) == 0) {
			if(1) {
				if(Scheduler::instance().clock() > 50) {
					misbehavior = sma_detect_misbehavior();
				}
				if((misbhReact_)) {
					if(misbehavior) {
						//printf("\nClock: %f NodeID: %d misbehavior: %f",Scheduler::instance().clock(),(Address::instance().get_nodeaddr(((MobileNode*)(netif_->node()))->nodeid())),misbehavior); 
						misbhReactionFactor_ = misbehavior;
						misbhType_ = MISBH_REACT_TYPE;
						misbhValue_ = (int)misbhReactionFactor_;
						misbhReactCount_ = misbhReactCount_ + 2;
					} else if(misbhReactCount_) {
						misbhType_ = MISBH_REACT_TYPE;
						misbhValue_ = (int)(MISBH_REACT_VALUE_DEFAULT/2);
						misbhReactCount_ = misbhReactCount_ - 1;
					} else {
						misbhType_ = 0;
						misbhValue_ = 0;
					}
				}
			}
		}
	}
	/*
	 * Address Filtering
	 */
//####//
void
Mac802_11::recvCTS(Packet *p)
{
	double ipttime=0.0;
.
.
.
	ssrc_ = 0;

	misbhSmaPacketCount_++;
	ipttime = (Scheduler::instance().clock() - misbhSmaPrevTime_);
	if(misbhSmaPacketCount_ > 1) {
		if((misbhSmaPacketCount_-1) < (MISBH_SMA_PERIOD)) {
			misbhSmaIpttimeTemp[((int)(misbhSmaPacketCount_-1)) - 1] = ipttime;
			misbhSmaSumIpttime_ = misbhSmaSumIpttime_ + ipttime;
		} else if((misbhSmaPacketCount_-1) == (MISBH_SMA_PERIOD)) {
			misbhSmaIpttimeTemp[(MISBH_SMA_PERIOD) - 1] = ipttime;
			misbhSmaSumIpttime_ = misbhSmaSumIpttime_ + ipttime;
			misbhSmaIpttime_ = misbhSmaSumIpttime_ / MISBH_SMA_PERIOD;
		} else if((misbhSmaPacketCount_-1) > (MISBH_SMA_PERIOD)) {
			if(((int)fmod((misbhSmaPacketCount_-1),(MISBH_SMA_PERIOD))) == 0) {
				misbhSmaSumIpttime_ = misbhSmaSumIpttime_ - misbhSmaIpttimeTemp[((int)fmod((misbhSmaPacketCount_-1),(MISBH_SMA_PERIOD))) + (MISBH_SMA_PERIOD- 1)];
				misbhSmaIpttimeTemp[((int)fmod((misbhSmaPacketCount_-1),(MISBH_SMA_PERIOD))) + (MISBH_SMA_PERIOD - 1)] = (Scheduler::instance().clock() - misbhSmaPrevTime_);
			} else {
				misbhSmaSumIpttime_ = misbhSmaSumIpttime_ - misbhSmaIpttimeTemp[((int)fmod((misbhSmaPacketCount_-1),(MISBH_SMA_PERIOD))) - 1];
				misbhSmaIpttimeTemp[((int)fmod((misbhSmaPacketCount_-1),(MISBH_SMA_PERIOD))) - 1] = ipttime;
			}
			misbhSmaSumIpttime_ = misbhSmaSumIpttime_ + ipttime;
			misbhSmaIpttime_ = misbhSmaSumIpttime_ / MISBH_SMA_PERIOD;
		}
	}
	misbhSmaPrevTime_ = Scheduler::instance().clock();

	tx_resume();
//####//
//At the end of the file
void Mac802_11::calc_neighbor_sma_ipttime(int src)
{
	struct misbh_neighborll *current_node=NULL,*temp_node=NULL;
	int node_found=0;
	current_node = neighborll_head;
	if(current_node == NULL) {
		node_found = 1;
		current_node = (struct misbh_neighborll *)malloc(sizeof(struct misbh_neighborll));
		current_node->index = src;
		current_node->sma_packet_count=1;
		current_node->sma_previous_pkt_rx_time = Scheduler::instance().clock();
		current_node->sma_sum_ipttime = 0.0;
		current_node->sma_ipttime = 0.0;
		current_node->sma_ratio = 0.0;
		current_node->misbehavior = 0;
		current_node->sma_difference=0;
		current_node->sma_var_sum=0;
		current_node->sma_var=0;
		current_node->sma_sd=0;

		current_node->next = NULL;
		neighborll_head = current_node;
		return;
	}
	while(current_node != NULL) {
		if(src == current_node->index) {
			node_found = 1;
			current_node->sma_packet_count++;
			if((current_node->sma_packet_count -1) < (MISBH_SMA_PERIOD)) {
				current_node->SmaTemp[((int)(current_node->sma_packet_count-1)) - 1] = (Scheduler::instance().clock() - current_node->sma_previous_pkt_rx_time);
				current_node->sma_sum_ipttime = current_node->sma_sum_ipttime + (Scheduler::instance().clock() - current_node->sma_previous_pkt_rx_time);
			} else if(((current_node->sma_packet_count-1)) == (MISBH_SMA_PERIOD)) {
				current_node->SmaTemp[(MISBH_SMA_PERIOD) - 1] = (Scheduler::instance().clock() - current_node->sma_previous_pkt_rx_time);
				current_node->sma_sum_ipttime = current_node->sma_sum_ipttime + (Scheduler::instance().clock() - current_node->sma_previous_pkt_rx_time);
				current_node->sma_ipttime = current_node->sma_sum_ipttime / MISBH_SMA_PERIOD;
			} else if(((current_node->sma_packet_count-1)) > (MISBH_SMA_PERIOD)) {
				if(((int)fmod(((current_node->sma_packet_count-1)),(MISBH_SMA_PERIOD))) == 0) {
					current_node->sma_sum_ipttime = current_node->sma_sum_ipttime - current_node->SmaTemp[((int)fmod(((current_node->sma_packet_count-1)),(MISBH_SMA_PERIOD))) + (MISBH_SMA_PERIOD- 1)];
					current_node->SmaTemp[((int)fmod(((current_node->sma_packet_count-1)),(MISBH_SMA_PERIOD))) + (MISBH_SMA_PERIOD - 1)] = (Scheduler::instance().clock() - current_node->sma_previous_pkt_rx_time);
				} else {
					current_node->sma_sum_ipttime = current_node->sma_sum_ipttime - current_node->SmaTemp[((int)fmod(((current_node->sma_packet_count-1)),(MISBH_SMA_PERIOD))) - 1];
					current_node->SmaTemp[((int)fmod(((current_node->sma_packet_count-1)),(MISBH_SMA_PERIOD))) - 1] = (Scheduler::instance().clock() - current_node->sma_previous_pkt_rx_time);
				}
				current_node->sma_sum_ipttime = current_node->sma_sum_ipttime + (Scheduler::instance().clock() - current_node->sma_previous_pkt_rx_time);
				current_node->sma_ipttime = current_node->sma_sum_ipttime / MISBH_SMA_PERIOD;
			}
			current_node->sma_previous_pkt_rx_time = Scheduler::instance().clock();
			if(misbhSmaIpttime_ && current_node->sma_ipttime) {
				current_node->sma_ratio = misbhSmaIpttime_ / current_node->sma_ipttime;
				misbhMaxIptRatio_ = current_node->sma_ratio > misbhMaxIptRatio_ ? current_node->sma_ratio : misbhMaxIptRatio_;
				misbhMinIptRatio_ = current_node->sma_ratio < misbhMinIptRatio_ ? current_node->sma_ratio : misbhMinIptRatio_;
			}
			break;
		}
		if(current_node->next != NULL) {
			current_node = current_node->next;
		} else {
			break;
		}
	}
	if(node_found == 0) {
		temp_node = (struct misbh_neighborll *)malloc(sizeof(struct misbh_neighborll));
		temp_node->index = src;
		temp_node->sma_packet_count=1;
		temp_node->sma_previous_pkt_rx_time = Scheduler::instance().clock();
		temp_node->sma_sum_ipttime = 0.0;
		temp_node->sma_ipttime = 0.0;
		temp_node->sma_ratio = 0.0;
		temp_node->misbehavior = 0;
		temp_node->sma_difference=0;
		temp_node->sma_var_sum=0;
		temp_node->sma_var=0;
		temp_node->sma_sd=0;
		temp_node->next = NULL;

		current_node->next = temp_node;
	}
}

double Mac802_11::sma_detect_misbehavior()
{
	int node_count=0,misbh_count=0;
	double misbehavior=0,kvalue=0.0,cwoptimal=0.0;
	struct misbh_neighborll *current_node=NULL;

	current_node = neighborll_head;
	if(current_node == NULL) {
		return 0;
	}
	while(current_node != NULL) {
		node_count++;
		if(((current_node->sma_ipttime * 1.25) < (misbhSmaIpttime_)) && (misbhSmaIpttime_) && (current_node->sma_ipttime)){
			current_node->misbehavior=1;
			misbehavior = (misbhSmaIpttime_ / current_node->sma_ipttime) > misbehavior ? (misbhSmaIpttime_ / current_node->sma_ipttime) : misbehavior;
			misbh_count++;
		}
		current_node = current_node->next;
	}
	misbehavior = misbehavior > 1 ? (1/misbehavior) : 0;
	if(!misbehavior) {
		return 0; //can be changed to 1 to indicate no misbehavior
	}
	//code below is part of reaction and can be moved to the function where this is function is called. 
	kvalue = sqrt(2 * ((txtime(phymib_.getRTSlen(),basicRate_) + DSSS_MaxPropagationDelay + phymib_.getDIFS())/(phymib_.getSlotTime())));
	//printf("\n t_rts: %f tau: %f DIFS %f kvalue %f",(txtime(phymib_.getRTSlen(),basicRate_)),DSSS_MaxPropagationDelay,phymib_.getDIFS(),kvalue);
	cwoptimal = ((node_count + 1)) * kvalue;
	misbehavior = ((cwoptimal * (node_count*node_count) * (misbehavior * misbehavior)))  / (200);
	misbehavior = misbehavior < 3 ? 3 : misbehavior;

	return misbehavior;
}
void Mac802_11::print_neighborllist()
{
	struct misbh_neighborll *current_node=NULL;
	current_node = neighborll_head;

	while(current_node != NULL){
		if((Address::instance().get_nodeaddr(((MobileNode*)(netif_->node()))->nodeid())) == 2) {
			printf("\nNodeID: %d ownipt: %f neighborid: %d neighboript: %f",(Address::instance().get_nodeaddr(((MobileNode*)(netif_->node()))->nodeid())),misbhSmaIpttime_,current_node->index,current_node->sma_ipttime_temp);
		}
		if(current_node->next != NULL) {
			current_node = current_node->next;
		} else {
			break;
		}
	}

}
//####//
