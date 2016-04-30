using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System;

public class GameManager : MonoBehaviour {

    public bool playerAlive;
    public GameObject playerPrefab;
    public Text currScoreText;
    public Text finalScoreText;
    public Text highscoresText;
    //Menu objects
    public Transform mainMenu, optionMenu, controls, pauseMenu, highscoresMenu, deathMenu, textCanvas, settingsMenu, creditsMenu, helpMenu;
    public Transform[] menus;

    private float timeElapsed = 0f;
    private float bestTime = 0f;
    private bool paused;
    private TimeManager timeManager;
    private GameObject player;
    private GameObject floor;
    private Spawner spawner;

    private int highscoresCount = 3;

    void Awake()
    {
        floor = GameObject.Find("Foreground");
        spawner = GameObject.Find("Spawner").GetComponent<Spawner>();
        timeManager = GetComponent<TimeManager>();
    }

    void Start()
    {

        var floorHeight = floor.transform.localScale.y;

        var pos = floor.transform.position;
        pos.x = 0;
        pos.y = -((Screen.height / PixelPerfectCamera.pixelsToUints) / 2) + (floorHeight / 2);
        floor.transform.position = pos;

        spawner.active = false;

        Time.timeScale = 0;

        //Setup Menus

        menus = new Transform[] {mainMenu, optionMenu, controls, pauseMenu, highscoresMenu, deathMenu, textCanvas, settingsMenu, creditsMenu, helpMenu};

        foreach (Transform m in menus)
        {
            m.gameObject.SetActive(false);
        }

        mainMenu.gameObject.SetActive(true);

        //Retrieves best time from unitys save feature
        bestTime = PlayerPrefs.GetFloat("BestTime");

        UpdateHighscoresText();

    }

	// Update is called once per frame
	void Update ()
    {
        if(!playerAlive)
        {
            textCanvas.gameObject.SetActive(false);
        }
        else
        {

            //Displays Score
            timeElapsed += Time.deltaTime;
            currScoreText.text = "TIME: " + FormatTime(timeElapsed);
        }
	}

    //Handles player death
    void OnPlayerKilled()
    {
        spawner.active = false;

        var playerDestroyScript = player.GetComponent<DestroyOffscreen>();
        playerDestroyScript.DestroyCallback -= OnPlayerKilled;

        player.GetComponent<Rigidbody2D>().velocity = Vector2.zero;

        ManipTime(0, .1f);
        playerAlive = false;

        //Updates best score on player death
        if (timeElapsed > bestTime)
        {
            bestTime = timeElapsed;
            PlayerPrefs.SetFloat("BestTime", bestTime);
        }

        //Displays final score on death screen
        finalScoreText.text = "TIME: " + FormatTime(timeElapsed) + "\nBEST: " + FormatTime(bestTime);

        CheckAndUpdateHighscores(timeElapsed);

        ActivateDeathMenu();
}

   //Resets game only if player is not alive
   public void ResetGame()
    {
        ManipTime(1, .1f);

        spawner.active = true;

        if (!playerAlive)
        {
            
            player = GameObjectUtil.Instantiate(playerPrefab, new Vector3(0, (Screen.height / PixelPerfectCamera.pixelsToUints) / 2 + 100, 0));
            var playerDestroyScript = player.GetComponent<DestroyOffscreen>();
            playerDestroyScript.DestroyCallback += OnPlayerKilled;
            playerAlive = true;
            timeElapsed = 0f;
        }
        else
        {
            player.transform.position =  new Vector3(0, (Screen.height / PixelPerfectCamera.pixelsToUints) / 2 + 100, 0);
            playerAlive = true;
            timeElapsed = 0f;
        }
    }

    string FormatTime(float value)
    {
        TimeSpan t = TimeSpan.FromSeconds(value);
        return string.Format("{0:D2}:{1:D2}", t.Minutes, t.Seconds);
    }

    public void ManipTime(float newTime, float duration)
    {
        timeManager.ManipulateTime(newTime, duration);
    }

    public void QuitGame()
    {
        Application.Quit();
    }

    public void PlayClicked(bool clicked)
    {
        if (clicked)
        {
            textCanvas.gameObject.SetActive(clicked);
            mainMenu.gameObject.SetActive(!clicked);
            deathMenu.gameObject.SetActive(!clicked);
            controls.gameObject.SetActive(clicked);
            ResetGame();
        }
    }

    public void OptionsClicked(bool clicked)
    {
        optionMenu.gameObject.SetActive(clicked);

        if (!paused)
        {
            mainMenu.gameObject.SetActive(!clicked);
        }
        else
        {
            pauseMenu.gameObject.SetActive(!clicked);
        }
    }

    // OPTIONS - Submenus are below =====================
    public void SettingsClicked(bool clicked)
    {
        settingsMenu.gameObject.SetActive(clicked);
        optionMenu.gameObject.SetActive(!clicked);
    }

    public void HelpClicked(bool clicked)
    {
        helpMenu.gameObject.SetActive(clicked);
        optionMenu.gameObject.SetActive(!clicked);
    }

    public void CreditsClicked(bool clicked)
    {
        creditsMenu.gameObject.SetActive(clicked);
        optionMenu.gameObject.SetActive(!clicked);
    }
    // end of OPTIONS - Submenus ========================

    public void PauseClicked(bool clicked)
    {
        if (clicked)
        {
            paused = true;
            pauseMenu.gameObject.SetActive(clicked);

            ManipTime(0, 0.5f);
        }
        else
        {
            paused = false;
            ManipTime(1, 1);
            pauseMenu.gameObject.SetActive(clicked);
        }
    }

    public void MainMenuClicked(bool clicked)
    {
        if (clicked && paused)
        {
            paused = false;
            pauseMenu.gameObject.SetActive(!clicked);
            mainMenu.gameObject.SetActive(clicked);
        }
        else if (clicked && !playerAlive)
        {
            deathMenu.gameObject.SetActive(!clicked);
            mainMenu.gameObject.SetActive(clicked);
        }
    }

    public void ResetScoresClicked()
    {
        PlayerPrefs.DeleteAll();
    }

    public void HighscoresClicked(bool clicked)
    {
        mainMenu.gameObject.SetActive(!clicked);
        highscoresMenu.gameObject.SetActive(clicked);
    }

    public void ActivateDeathMenu()
    {
        deathMenu.gameObject.SetActive(true);
    }

    public void CheckAndUpdateHighscores(float time)
    {
        float temp = time;
        bool swap = false;

        for (int i = 0; i < highscoresCount; i++)
        {
            float prevTime = PlayerPrefs.GetFloat("Highscore" + i);

            if (swap)
            {
                PlayerPrefs.SetFloat("Highscore" + i,temp);
                temp = prevTime;
            }
            else if (time > prevTime)
            {
                PlayerPrefs.SetFloat("Highscore" + i,time);
                swap = true;
                temp = prevTime;
            }
        }

        UpdateHighscoresText();
        
    }

    public void UpdateHighscoresText() {
        highscoresText.text = 
            "1st Place: " + FormatTime(PlayerPrefs.GetFloat("Highscore0")) + "\n" +
            "2nd Place: " + FormatTime(PlayerPrefs.GetFloat("Highscore1")) + "\n" +
            "3rd Place: " + FormatTime(PlayerPrefs.GetFloat("Highscore2"));
    }

}
