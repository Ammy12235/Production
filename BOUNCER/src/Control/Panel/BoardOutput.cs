using NUnit.Framework;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Video;
using System.Collections.Generic;

public class BoardOutput : MonoBehaviour
{

    private BoardInput previousClips;
    private BoardInput currentClips;
    private VideoPlayer videoPlayer;
    [SerializeField] private GameObject noSignal;
    [SerializeField] private float transitionTime = 2;
    private List<VideoClip> clips;
    private int clipIndex = 0;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        noSignal.SetActive(false);
        videoPlayer = GetComponent<VideoPlayer>();
        videoPlayer.loopPointReached += LoopPointReached;
    }

    // Update is called once per frame
    void Update()
    {
       
    }

    private void LoopPointReached(VideoPlayer player)
    {
        videoPlayer.Pause();
        clipIndex++;
        if (clipIndex>=clips.Count) clipIndex = 0;
        player.clip = clips[clipIndex];//次に登録されたクリップを再生
        videoPlayer.Play();
    }

    public void NotifyInput(BoardInput input)//選択肢が入れ替わったら呼ばれる
    {
        if (input!=null)
        {
            currentClips = input;
            clips = currentClips.GetVideoClips();
            if(clips.Count==0||clips==null)//入力にクリップが登録されていなかったら
            {
                videoPlayer.gameObject.SetActive(false);
                noSignal.SetActive(true);//NoSignalメッセージを出す
                return;
            }
            else
            {
                videoPlayer.gameObject.SetActive(true);
                noSignal.SetActive(false);
            }
            videoPlayer.Pause();
            clipIndex = 0;
            videoPlayer.clip = clips[clipIndex];
            videoPlayer.Play();
        }
    }
}
