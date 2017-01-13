#include <string>
#include <vector>
#include "boost/signals2/signal.hpp"
#include <boost/shared_ptr.hpp>




class RouteSignal{

	typedef boost::signals2::signal<void(float)> V_Signal;
	//typedef boost::shared_ptr<V_Signal> SignalRef;
	typedef V_Signal::slot_type m_slot_type;


public:
	//template <typename T>
	RouteSignal( const m_slot_type &suscriber , string fNName, string fNField ){
		this->fromNodeName = string();
		this->fromNodeField = string();
		m_signal.connect(suscriber);
		
	}

	string getFromNodeName() { return this->fromNodeName; }
	string getFromFieldName() { return this->fromNodeField; }

private:

	string fromNodeName;
	string fromNodeField;
	V_Signal m_signal;
};

template<class T>
class RouteSignalT{

	typedef boost::signals2::signal<void(T &)> V_Signal;
	typedef typename V_Signal::slot_type m_slot_type;
	//typedef typename V_Signal::slot_type m_slot_type;



public:
	template <class T>
	RouteSignalT( const m_slot_type &suscriber ){
		this->fromNodeName = string();
		this->fromNodeField = string();
		m_signal.connect(suscriber);
		
	}

	string getFromNodeName() { return this->fromNodeName; }
	string getFromFieldName() { return this->fromNodeField; }

private:

	string fromNodeName;
	string fromNodeField;
	V_Signal m_signal;
};


//template<typename T>
//class RouteSignal{
//
//    //typedef boost::signals2::signal<void ( T &, string, string )> V_Signal;
//	typedef boost::signals2::signal<void(T&)> V_Signal;
//	typedef boost::shared_ptr<V_Signal> SignalRef;
//	typedef typename V_Signal::slot_type m_slot_type;
//
//
//public:
//	template <typename T>
//	RouteSignal( const m_slot_type &suscriber ){
//		this->fromNodeName = string();
//		this->fromNodeField = string();
//		m_signal.connect(suscriber);
//		/*vSig = vector<T&>();
//		m_signalRef = NULL;*/
//		/*m_signalRef = SignalRef( new V_Signal() );
//		m_signalRef->connect(suscriber);*/
//	}
//
//	//RouteSignal(const string sFN /*= string()*/ , const string sFF  /*= string()*/ ){
//	//	this->fromNodeName = sFN;
//	//	this->fromNodeField = sFF;
//	//	vSig = vector<T&>();
//	//	m_signalRef = NULL;
//	//}
////template <typename T> void MyQueue<T> ::Add(T const &d)
////{
////     data.push_back(d);
////}
//
//	//template <typename T>
//	// void addCallback( const m_slot_type &suscriber/*, string sFN, string sFF */){
//	//	/*this->fromNodeName = sFN;
//	//	this->fromNodeField = sFF;*/
//	//	m_signalRef = SignalRef( new V_Signal() );
//	//	m_signalRef->connect(suscriber);
//	//	//mSigRef.push_back(m_signalRef);
//	//}
//
//	string getFromNodeName() { return this->fromNodeName; }
//	string getFromFieldName() { return this->fromNodeField; }
//	/*boost::signals2::connection connect( const V_Signal::slot_type &suscriber, string toNode, toField){
//		return m_signal.connect(suscriber);
//	}*/
//
//private:
//	  V_Signal m_signal;
//	SignalRef m_signalRef;
//	string fromNodeName;
//	string fromNodeField;
//	std::vector<T> data;
//
//};


//class SFFloatRouteSignal : public RouteSignal {
//
//    typedef boost::signals2::signal<void (float)> V_Signal;
//
//public:
//
//	SFFloatRouteSignal(string sFN = string() , string sFF  = string()):RouteSignal(sFN,sFF){};
//
//	boost::signals2::connection connect( const V_Signal::slot_type &suscriber){
//		return m_signal.connect(suscriber);
//	}
//
//
//private:
//    V_Signal m_signal;
//
//};

//template<class T>
//class RouteSignal
//{
//    typedef boost::signals2::signal<void (T &)> OnReceive;
//	typedef typename OnReceive::slot_type slot_type;
//
//
//public:
//    RouteSignal(const slot_type &_slot)
//    {
//       //m_onReceive.connect(_slot);
//    }
//    //inline virtual RouteSignal(){};
//
//    void Send(T &_val)
//    {
//       m_onReceive(_val);
//    }
//private:
//    OnReceive m_onReceive;
//};




//class SFVec3fRouteSignal : public RouteSignal{
//	
//	typedef boost::signals2::signal< void(float,float,float) > Sfvec3f_Signal;
//
//	public:
//
//		SFVec3fRouteSignal():m_signal(){};
//		SFVec3fRouteSignal(const Sfvec3f_Signal::slot_type &subscriber, const string fromNodeName, const string fromFieldName):m_signal(){
//			//timeSig.connect( boost::bind( (&X3dWrapper::updateSFVec3fNode), this, _1 , string(fromNodeN), string(fromFieldN) ) );
//			//m_signal.connect( boost::bind(subscriber, _1,_2,_3,fromNodeName,  fromFiledName) );
//			/*this->fromNodeName = fromNodeName;
//			this->fromFieldName = fromFieldName;*/
//			//m_signal.connect( boost::bind(subscriber, _1,_2,_3,fromNodeName,fromFieldName) );
//			//m_signal.connect( subscriber, _1,_2,_3,fromNodeName,fromFieldName);
//			m_signal.connect( boost::bind(subscriber, _1,_2,_3) );
//		}
//
//		void send( const float x,const float y,const float z){
//			m_signal(x,y,z);
//		}
//
//	private:
//
//	Sfvec3f_Signal m_signal;
//	/*string fromNodeName;
//	string fromFieldName;*/
//
//};
