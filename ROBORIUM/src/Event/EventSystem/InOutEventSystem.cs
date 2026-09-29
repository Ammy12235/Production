using System.Collections.Generic;
using UnityEngine;


public class InOutEventSystem : Singleton<InOutEventSystem>
{
    private LinkedList<IEvent> eventLinkedList=new();
    
    protected override bool UseDontDestroyOnLoad => false;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {

    }

    public void SetEvent(IEvent eventObj)
    {
        //連結リストにイベントを追加
        eventLinkedList.AddLast(eventObj);

        //イベントの初期化
        eventObj.Init();

#if UNITY_EDITOR

        CreateDebugDraw(eventObj);
#endif


    }

#if UNITY_EDITOR
    public void CreateDebugDraw(IEvent eventObj)
    {
        GameObject debugObj = new GameObject("EventDebugLine");
        var debugLineDrawer = debugObj.AddComponent<EventLineDrawer>();
        var actuator = eventObj.GetOwnerActuator();
        if (actuator != null)
        {
            debugLineDrawer.SetPair(eventObj.gameObject.transform.position, actuator.transform.position);
        }


    }

#endif

    // Update is called once per frame
    void Update()
    {
        //もしイベントキューにイベントがある場合は、イベントを処理する
        if (eventLinkedList != null && eventLinkedList.Count != 0)
        {
            var node = eventLinkedList.First;
            while (node != null)
            {
                var next = node.Next;
                var eventInfo = node.Value; // structをコピー

                if (eventInfo.IsRunning() == true && eventInfo.IsFinished()==false)//イベントを実行する
                {
                    eventInfo.Run();
                }
                else if (eventInfo.IsFinished()==true)//もしイベントが終了している場合は、イベントキューから削除する
                {
                    eventLinkedList.Remove(node);
                }

                node = next;
            }
        }
    }
}
