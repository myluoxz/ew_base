
#include "ewc_base/plugin/plugin_expression.h"
#include "ewc_base/plugin/plugin_manager.h"
#include "ewc_base/evt/evt_manager.h"
#include "ewc_base/wnd/wnd_updator.h"
#include "ewc_base/wnd/wnd_model.h"

EW_ENTER


class IExprData
{
public:
	String expr_expresion;
	String expr_result;
	Executor lexer;

	IExprData()
	{
		lexer.set_time_limit(TimeSpan::MilliSeconds(1000));
		lexer.set_page_limit(1024*1024*16);
	}
	
	static IExprData& current()
	{
		static IExprData gInstance;
		return gInstance;
	}
};

PluginExpression::PluginExpression(WndManager& w):PluginCommon(w,"Plugin.Expression")
{

}

bool PluginExpression::OnAttach()
{
	IExprData& idat(IExprData::current());

	wm.evtmgr.append(new WndModelScript("Model.Expr","scripting/ui/dlg_expression.ewsl"));
	wm.evtmgr.append(new EvtCommandShowModel(_kT("Dlg.Expr"),"Model.Expr"));

	wm.evtmgr.link("expr.expression",idat.expr_expresion);
	wm.evtmgr.link("expr.result",idat.expr_result);


	class expr_timer : public EvtCommandTimer
	{
	public:
		expr_timer():EvtCommandTimer("expr.timer")
		{
			AttachEvent("expr.expression");
		}
		bool OnWndEvent(IWndParam& cmd,int phase)
		{
			if(phase==IDefs::PHASE_POSTCALL && cmd.action==IDefs::ACTION_VALUE_CHANGING)
			{
				StdExecuteEx(1000);
			}
			return true;
		}
		bool DoCmdExecute(ICmdParam&)
		{
			WndManager& wm(WndManager::current());
			wm.evtmgr["expr.expression"].WndExecuteEx(IDefs::ACTION_TRANSFER2MODEL);
			IExprData& idat(IExprData::current());
			LoggerSwap logger;//disable the error messages
			
			this_logger().Cache(true);

			if(idat.expr_expresion.empty())
			{
				idat.expr_result="";
			}
			else if(!idat.lexer.eval(idat.expr_expresion,idat.expr_result))
			{
				arr_1t<LogRecord> msgs;
				this_logger().Clear(msgs);

				if (!msgs.empty())
				{
					idat.expr_result = "ERROR: " + msgs[0].m_sMessage;
				}
				else
				{
					idat.expr_result="---";	
				}

		
			}
			wm.evtmgr["expr.result"].WndExecuteEx(IDefs::ACTION_TRANSFER2WINDOW);
			return true;
		}
	};

	wm.evtmgr.append(new expr_timer);

	//class expr_ok : public EvtCommand
	//{
	//public:
	//	expr_ok():EvtCommand("expr.ok")
	//	{				
	//	}
	//	bool DoCmdExecute(ICmdParam&)
	//	{
	//		WndManager::current().evtmgr["Model.Expr"].StdExecuteEx(0,IDefs::BTN_OK);
	//		return true;
	//	}
	//};
	//class expr_cancel : public EvtCommand
	//{
	//public:
	//	expr_cancel() :EvtCommand("expr.cancel")
	//	{

	//	}
	//	bool DoCmdExecute(ICmdParam&)
	//	{
	//		WndManager::current().evtmgr["Model.Expr"].StdExecuteEx(0,IDefs::BTN_CANCEL);
	//		return true;
	//	}
	//};


	//wm.evtmgr.append(new expr_cancel);
	//wm.evtmgr.append(new expr_ok);
	return true;
}

IMPLEMENT_IPLUGIN(PluginExpression)

EW_LEAVE
