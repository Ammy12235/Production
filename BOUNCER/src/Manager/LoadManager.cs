using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class LoadManager : MonoBehaviour
{

    public static LoadManager instance;

    [SerializeField] private float waitTime = 5;
    float timer = 0.0f;

  
    //　シーンロード中に表示するUI画面
    [SerializeField]
    private GameObject loadUI;

    private Image blackImage;
   
    bool isReady = false;

    private void Awake()
    {

        if (instance == null)
        {
            instance = this;
            //DontDestroyOnLoad(gameObject); // �V�[���ԂŃ}�l�[�W���[��ێ�����ꍇ
        }
        else
        {
            Destroy(gameObject);
        }
    }
    private void Start()
    {
        loadUI.SetActive(false);
     
    }

    public void NextScene(string nextSceneName)
    {
        //　ロード画面UIをアクティブにする
        loadUI.SetActive(true);
       
        timer = 0.0f;
        isReady = false;
        //　コルーチンを開始
        StartCoroutine("LoadSceneAsync", nextSceneName);
    }

    IEnumerator LoadSceneAsync(string nextScene)
    {
        //　非同期動作で使用するAsyncOperation
        AsyncOperation operation = SceneManager.LoadSceneAsync(nextScene);
        operation.allowSceneActivation = false;

        while (!operation.isDone)
        {
            float progress = Mathf.Clamp01(operation.progress / 0.9f);//0.9しないと永遠にロードが終わらないらしい

            if (progress >= 1f)
            {
               
                yield return new WaitForSeconds(waitTime);
                
                operation.allowSceneActivation = true;
            }

            yield return null;
        }
    }
}