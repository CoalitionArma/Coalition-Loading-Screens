modded class SCR_LoadingScreenComponent
{
	ref COALLOAD_Screens m_Screen;
	ref RandomGenerator m_Random;

	ImageWidget m_Image1;
	ImageWidget m_Image2;

	int m_iActiveImage = 1;

	float m_fSwapTimer = 0;
	float m_fFadeTimer = 0;

	bool m_bFading = false;

	const float SWAP_INTERVAL = 3.0;
	const float FADE_DURATION = 1;

	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

		m_Random = new RandomGenerator();
		m_Random.SetSeed(System.GetTickCount());

		m_Image1 = ImageWidget.Cast(w.FindAnyWidget("LoadingScreen1"));
		m_Image2 = ImageWidget.Cast(w.FindAnyWidget("LoadingScreen2"));

		m_Screen = COALLOAD_Screens.Cast(
			BaseContainerTools.CreateInstanceFromContainer(
				BaseContainerTools.LoadContainer("{4D826049AC0264C6}configs/COALLOAD_Images.conf").GetResource().ToBaseContainer()
			)
		);

		if (!m_Image1 || !m_Image2 || !m_Screen || !m_Screen.m_aImages || m_Screen.m_aImages.Count() == 0)
			return;

		int index1 = m_Random.RandInt(0, m_Screen.m_aImages.Count());
		m_Image1.LoadImageTexture(0, m_Screen.m_aImages[index1]);
		m_Image1.SetImage(0);
		m_Image1.SetOpacity(1);

		int index2 = m_Random.RandInt(0, m_Screen.m_aImages.Count());
		m_Image2.LoadImageTexture(0, m_Screen.m_aImages[index2]);
		m_Image2.SetImage(0);
		m_Image2.SetOpacity(0);
		
		Widget coalLogo = w.FindAnyWidget("CoalLogo");
		if (coalLogo)
			coalLogo.SetOpacity(1);

		m_iActiveImage = 1;
	}

	override void Update(float timeSlice, float progress = 0, float minDurationRatio = 0)
	{
		super.Update(timeSlice, progress, minDurationRatio);

		if (!m_Image1 || !m_Image2 || !m_Screen || !m_Screen.m_aImages || m_Screen.m_aImages.Count() == 0)
			return;

		if (!m_bFading)
		{
			m_fSwapTimer += timeSlice;

			if (m_fSwapTimer >= SWAP_INTERVAL)
			{
				StartImageSwap();

				m_bFading = true;
				m_fFadeTimer = 0;
				m_fSwapTimer = 0;
			}

			return;
		}

		m_fFadeTimer += timeSlice;

		float fadeAlpha = Math.Clamp(1.0 - m_fFadeTimer / FADE_DURATION, 0.0, 1.0);

		if (m_iActiveImage == 1)
			m_Image1.SetOpacity(fadeAlpha);
		else
			m_Image2.SetOpacity(fadeAlpha);

		if (m_fFadeTimer >= FADE_DURATION)
			FinishImageSwap();
	}

	void StartImageSwap()
	{
		int newIndex = m_Random.RandInt(0, m_Screen.m_aImages.Count());
	
		if (m_iActiveImage == 1)
		{
			// New image behind
			m_Image2.LoadImageTexture(0, m_Screen.m_aImages[newIndex]);
			m_Image2.SetImage(0);
			m_Image2.SetOpacity(1);
			m_Image2.SetZOrder(-2);
	
			// Current image on top fading away
			m_Image1.SetZOrder(-1);
		}
		else
		{
			// New image behind
			m_Image1.LoadImageTexture(0, m_Screen.m_aImages[newIndex]);
			m_Image1.SetImage(0);
			m_Image1.SetOpacity(1);
			m_Image1.SetZOrder(-2);
	
			// Current image on top fading away
			m_Image2.SetZOrder(-1);
		}
	}

	void FinishImageSwap()
	{
		if (m_iActiveImage == 1)
		{
			m_Image1.SetOpacity(0);
	
			// New image becomes active
			m_Image2.SetOpacity(1);
	
			// Maintain ordering
			m_Image1.SetZOrder(-1);
			m_Image2.SetZOrder(-2);
	
			m_iActiveImage = 2;
		}
		else
		{
			m_Image2.SetOpacity(0);
	
			// New image becomes active
			m_Image1.SetOpacity(1);
	
			// Maintain ordering
			m_Image2.SetZOrder(-1);
			m_Image1.SetZOrder(-2);
	
			m_iActiveImage = 1;
		}
	
		m_bFading = false;
		m_fFadeTimer = 0;
	}
}

[BaseContainerProps(configRoot: true)]
class COALLOAD_Screens
{
	[Attribute(params: "edds")]
	ref array<ResourceName> m_aImages;
}