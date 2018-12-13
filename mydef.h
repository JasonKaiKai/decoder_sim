//Notation
#define         TRUE           1
#define         FALSE          0
#define   cycle          1000000  
#define   x_axis         15

//Timer definition
#define time_unit         1
#define   t_expire        50 * time_unit //Reporting time period for REPORTING event



//Credit Information
#define  initCredit            100000          //Initial credit in user account or recharge credit
#define  eventCreditUnit       1
//#define  m                     30             //Theta = m events' credit

//Arrival Traffic Expo
#define  lambda_MTC_unit  1
#define  lambda_MTC_scale 0.5
#define  lambda_MTC       lambda_MTC_unit * lambda_MTC_scale

//Random user check
#define  lambda_user_unit             0.01           //Random_check arrival rate
#define  lambda_user_scale            1
#define  lambda_user                  lambda_user_unit * lambda_user_scale

//Specific define each event type
// #define		ARRIVAL             1
// #define		CREDIT_RESERVATION  2
// #define     CREDIT_CONSUMED     3
// #define		TIMER_EXPIRY    	4
// #define		RANDOM_CHECK    	5