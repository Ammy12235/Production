using UnityEngine;
using UnityEngine.SceneManagement;

public class SceneJumpOnPress : MonoBehaviour
{
    [Header("遷移先シーン（Build Settings に登録しておく）")]
    [SerializeField] private string sceneName;

    [SerializeField] private LoadSceneMode loadMode = LoadSceneMode.Single;

#if UNITY_EDITOR
    // エディタ上でシーンアセットを指定→自動で名前を反映（実行時は string を使用）
    [SerializeField] private UnityEditor.SceneAsset sceneAsset;
    private void OnValidate()
    {
        if (sceneAsset != null)
        {
            // Build 設定に入っている前提
            sceneName = sceneAsset.name;
        }
    }
#endif

    private void Update()
    {
        if (PressedSpaceOrAButton())
        {
            if (string.IsNullOrEmpty(sceneName))
            {
                Debug.LogError("[SceneJumpOnPress] sceneName が未設定です。インスペクターで指定してください。");
                return;
            }

            SceneManager.LoadScene(sceneName, loadMode);
        }
    }

    private bool PressedSpaceOrAButton()
    {
#if ENABLE_INPUT_SYSTEM && !ENABLE_LEGACY_INPUT_MANAGER
        var kb = UnityEngine.InputSystem.Keyboard.current;
        if (kb != null && kb.spaceKey.wasPressedThisFrame)
        if (gp != null && gp.buttonEast.wasPressedThisFrame) return true;
        return false;
#else
        // ── 旧 Input（Input Manager）を使う場合 ──
        // Space
        if (Input.GetKeyDown(KeyCode.Space)) return true;

        // 多くのコントローラで東ボタンは JoystickButton1 にマップされることが多い
        // 必要なら機種に合わせてボタン番号を調整してください
        if (Input.GetKeyDown(KeyCode.JoystickButton1)) return true;

        // Submit を使っても OK（Input Manager の設定次第）
        if (Input.GetButtonDown("Submit")) return true;

        return false;
#endif
    }
}