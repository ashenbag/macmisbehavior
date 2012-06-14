#define MAC_Subtype_ProbeReq	0x04
#define MAC_Subtype_ProbeRep	0x05
#define MISBH_SMA_PERIOD	500
#define MISBH_REACT_TYPE	5
#define MISBH_REACT_VALUE_DEFAULT ((phymib_.getCWMin())/2) //For CWFix misbehavior reaction
//####//
struct priority_queue {
	int frame_priority;
	struct priority_queue *next;
};

/* structure for maintaing neighbor IPT Time. Neighbors are represented by nodes of linked list. 
ema_ratio is Exponential Moving Average is used for detection. */
struct misbh_neighborll {
	int index;
	double sma_packet_count;
	double sma_previous_pkt_rx_time;
	double sma_ipttime;
	double sma_sum_ipttime;
	double sma_ipttime_temp;
	double sma_ipttime_ratio;
	double sma_ratio;
	double sma_difference;
	double sma_var_sum;
	double sma_var;
	double sma_sd;
	double SmaTemp[MISBH_SMA_PERIOD];
	int misbehavior;
	struct misbh_neighborll *next;
};
//####//
class PHY_MIB {
public:
	PHY_MIB(Mac802_11 *parent);

	inline u_int32_t getCWMin() { return(CWMin); }
	inline u_int32_t getCWMax() { return(CWMax); }
	inline int getMisbhType() { return(MisbhType); }
	inline double getMisbhValue() { return(MisbhValue); }
	inline int getMisbhTime() { return(MisbhTime); }
	inline int getMisbhReact() { return(MisbhReact); }
//####//
	u_int32_t	PreambleLength;
	u_int32_t	PLCPHeaderLength;
	double		PLCPDataRate;
	int		MisbhType;
	double		MisbhValue;
	double		MisbhTime;
	int		MisbhReact;
};
//####//
	inline void set_nav(u_int16_t us) {
		double now = Scheduler::instance().clock();
		double t = us * 1e-6;
		if((now + t) > nav_) {
			nav_ = now + t;
			if(mhNav_.busy())
				mhNav_.stop();
			mhNav_.start(t);
		}
	}
	void calc_neighbor_sma_ipttime(int);
	double sma_detect_misbehavior();
	void print_neighborllist();
//####//
	struct ap_table	*ap_list;
	struct priority_queue *queue_head;
	struct misbh_neighborll *neighborll_head;
//####//
	u_int32_t	cw_;		// Contention Window
	u_int32_t	ssrc_;		// STA Short Retry Count
	u_int32_t	slrc_;		// STA Long Retry Count
	int		misbhType_;	//Misbehavior Type
	double		misbhValue_;	//Misbehavior Value
	double		misbhTime_;	//Misbehavior Time
	int		misbhReact_;	//Should the node react?
	double          misbhSmaPacketCount_; //Successfully sent packet. Counted based on the recevied ACK. 
	double          misbhSmaPrevTime_; //Time at which previous ACK was received
	double          misbhSmaIpttime_; //Moving Average Inter packet transmission time
	double          misbhSmaSumIpttime_; //Cumulative Inter packet transmission time for moving average
	double          misbhSmaVarSum_; //Variance Sum
	double          misbhSmaVar_; //Variance
	double          misbhSmaSD_;       //Standard Deviation
	double          misbhSmaIpttimeTemp[MISBH_SMA_PERIOD];
	double          misbhRxPacketCount_; //RX Packet count
	double          misbhReactCount_; //React count
	double          misbhReactionFactor_; //Reaction Factor
	int             misbhNeighborCount_; //Number of neighbors
	double          misbhMaxIptRatio_; //Maximum IPT Ratio
	double          misbhMinIptRatio_; //Minimum IPT Ratio
//####//
